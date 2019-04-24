#include <iostream>
#include <string.h>
#include <stdexcept>      // needed to throw errors
#include <stdio.h>        // needed for sprintf and possibly others
#include <unistd.h>       // UNIX standard function definitions
#include <fcntl.h>        // File control definitions
#include <errno.h>        // Error number definitions
//#include <termios.h>      // POSIX terminal control definitions
#include <vector>	      // for keeping track of devices
#include <time.h>         // used to set time-out of read function
//#include <sstream>        // used to concatinate strings
#include <sys/select.h>   // used to set timeout on serial read
#include <sys/ioctl.h>    // I/O contol needed for RS485
#include <linux/serial.h> // for RS485 control
#include <sys/socket.h>   // socket library
#include <netdb.h>        // used for ethernet communication

#include "device.h"
#include "write.h"

using namespace std;

int eth_connect(ethdevice *dev)
{
    bool debug_output = false;

    struct sockaddr_in serv_addr; // device address
    struct hostent *server; // pointer to host object

    if(debug_output == true) cout << "opening socket for " << dev->getname() << "...\n";

    // open socket
    dev->setsock(socket(AF_INET, SOCK_STREAM, 0));
    if (dev->getsock() < 0) return -1; // couldn't open socket

    struct timeval tv;
    tv.tv_sec = 3; // timeout in seconds
    tv.tv_usec = 0;
    setsockopt(dev->getsock(), SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv); // apply timeout settings

    if(debug_output == true)
    {
        cout << "using socket " << dev->getsock() << endl;
        cout << "getting host name...\n";
    }

    // parse host name
    server = gethostbyname(dev->gethostname().c_str());
    if (server == NULL) return -2; // host not found

    // zero the server address array and parse
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);

    if(debug_output == true) cout << "connecting to server...\n";

    // connect to server
    //serv_addr.sin_port = htons(portno);
    serv_addr.sin_port = htons(dev->getportno());
    if(debug_output == true)
    {
        cout << "port (host):" << dev->getportno() << endl;
        cout << "port (net):" << serv_addr.sin_port << endl;
    }
    if (connect(dev->getsock(),(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        return -3; // couldn't connect

    if(debug_output == true) cout << "done setting up socket.\n";
    return 0;
}

int eth_disconnect(ethdevice *dev)
{
    if(close(dev->getsock()) < 0) return -1; // close socket
    return 0;
}

string eth_IO(string cmd_in, ethdevice *dev) // send single command to ethernet device
{
    bool debug_output = false;

    int buff_size = 256;
    char buffer[buff_size];
    int n; // file descriptor, number of chars, tracking int

    if(debug_output == true) cout << "pointed to " << dev->getname() << endl;

    // intial read
    if(dev->getpreread() == true)
    {
        if(debug_output == true) cout << "performing initial read...";
	    bzero(buffer,buff_size);
        n = recv(dev->getsock(), &buffer[0], (buff_size-1), 0);
        if (n < 0) return "!ERR4"; // couldn't read socket
	}

    bzero(buffer,buff_size);
    strcpy(buffer,cmd_in.c_str());

    //for(int i=0; buffer[i] != '\0';i++){} // append <CR> and <LF>
    //buffer[i] = '\r';
    //buffer[i+1] = '\n';

    // write command to socket
    if(debug_output == true) cout << "writing:" << buffer << endl;
    n = send(dev->getsock(), &buffer[0], strlen(buffer), 0); //MSG_NOSIGNAL
    if(debug_output == true) cout << "send return #:" << n << endl;
    if (n < 0) return "!ERR5"; // couldn't write to socket

    if(dev->get_pause()) usleep(50000);

    if(debug_output == true) cout << "reading...\n";

    // zero the buffer and read from the socket
    bzero(buffer,buff_size);
    n = recv(dev->getsock(), &buffer[0], (buff_size-1), 0);
    if (n < 0) return "!ERR4"; // couldn't read socket
    if(debug_output == true) cout << "read: " << buffer << endl;

    return buffer;
}

string eth_read(ethdevice *dev) // send single command to ethernet device
{
    bool debug_output = false;

    int buff_size = 256;
    char buffer[buff_size];
    int n; // file descriptor, number of chars, tracking int

    if(debug_output == true)
    {
        cout << "pointed to " << dev->getname() << endl;
        cout << "reading...\n";
    }

    // zero the buffer and read from the socket
    bzero(buffer,buff_size);
    n = recv(dev->getsock(), &buffer[0], (buff_size-1), 0);
    if (n < 0) return "!ERR4"; // couldn't read socket
    if(debug_output == true) cout << "read: " << buffer << endl;

    return buffer;
}

string eth_write(string cmd_in, ethdevice *dev) // send single command to ethernet device
{
    bool debug_output = false;

    int buff_size = 256;
    char buffer[buff_size];
    int n; // file descriptor, number of chars, tracking int

    if(debug_output == true) cout << "pointed to " << dev->getname() << endl;

    bzero(buffer,buff_size);
    strcpy(buffer,cmd_in.c_str());

    //for(int i=0; buffer[i] != '\0';i++){} // append <CR> and <LF>
    //buffer[i] = '\r';
    //buffer[i+1] = '\n';

    // write command to socket
    if(debug_output == true) cout << "writing:" << buffer << endl;
    n = send(dev->getsock(), &buffer[0], strlen(buffer), MSG_NOSIGNAL);
    if (n < 0) return "!ERR5"; // couldn't write to socket
    if(debug_output == true) cout << "done writing.\n";

    return "DONE";
}

int eth_latch(ethdevice *dev)
{
    int buff_size = 256;
    char buffer[buff_size];
    int sockfd, n, i; // file descriptor, number of chars, tracking int
    struct sockaddr_in serv_addr; // device address
    struct hostent *server; // pointer to host object
    bool irdone = false; // track wether inital read just occured
    // this prevents a preread following the initial read

    // open socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) return -1; // couldn't open socket

    // parse host name
    //server = gethostbyname(hostname);
    server = gethostbyname(dev->gethostname().c_str());
    if (server == NULL) return -2; // host not found

    // zero the server address array and parse
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);

    // connect to server
    //serv_addr.sin_port = htons(portno);
    serv_addr.sin_port = htons(dev->getportno());
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        return -3; // couldn't connect

    // intial read
    if(dev->getpreread())
    {
        bzero(buffer,buff_size);
        n = recv(sockfd, &buffer[0], (buff_size-1), 0);
        if (n < 0) return -4; // couldn't read socket
    }

    bool hold = true;
    while(hold)
    {
        if(dev->getpreread() && irdone) // pre-read
        {
            bzero(buffer,buff_size);
            n = recv(sockfd, &buffer[0], (buff_size-1), 0);
            if (n < 0) return -5; // couldn't read socket (pre-read)
            printf("%.*s", n, buffer);
        }
        else if(!irdone) irdone = true;

        bzero(buffer,buff_size);
        char* useless = fgets(buffer,(buff_size-1),stdin); // take user input and write to buffer
        free(useless);

        if(buffer[0] != 'x' && buffer[0] != 'X') // check for escape keys
        {
            for(i=0; buffer[i] != '\0';i++){}
            buffer[i] = '\r';
            buffer[i+1] = '\n';

            // write command to socket
            n = send(sockfd, &buffer[0], strlen(buffer), MSG_NOSIGNAL);
            if (n < 0) return -6; // couldn't write to socket

            // zero the buffer and read from the socket
            bzero(buffer,buff_size);
            n = recv(sockfd, &buffer[0], (buff_size-1), 0);
            if (n < 0) return -7; // couldn't read socket

            printf("%.*s", n, buffer);
        }
        else hold = false;
    }
    if(close(sockfd) < 0) return -8;
    return 0;
}

int ser_connect(serdevice *dev)
{
    bool debug_output = false;

    if(debug_output == true) cout << "connecting serially to " << dev->getname() << endl;

    //struct termios sset_send, sset_recv;
    struct termios serset;// serset_prev;
    memset (&serset, 0, sizeof serset);

    // open the port
    //int dev->get_fd() = open( dev->getport().c_str(), O_RDWR| O_NOCTTY );
    dev->set_fd(open( dev->getport().c_str(), O_RDWR| O_NOCTTY ));
    if(dev->get_fd() < 0)
        return -1; // failed to open port 

    if(dev->using_str())
    {
        //int stat = dev->open_stream();
        //if(stat<0) return -2;
    }

    // initialize file descriptor sets (needed for read timeout)
    fd_set read_fds, write_fds, except_fds;
    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);
    FD_ZERO(&except_fds);
    FD_SET(dev->get_fd(), &read_fds);

    // load existing port attributes
    if (tcgetattr (dev->get_fd(), &serset) != 0)
        return -3; // failed to load existing port attributes

    // set baud rate
    cfsetospeed (&serset, dev->getbaud());
    cfsetispeed (&serset, dev->getbaud());

    // set various port attributes
    serset.c_cflag     &=  ~PARENB; // make 8n1
    serset.c_cflag     &=  ~CSTOPB;
    serset.c_cflag     &=  ~CSIZE;
    serset.c_cflag     |=  CS8;
    serset.c_cc[VMIN]   =  1; // previously 1
    serset.c_cc[VTIME]  =  5; // 0.5 seconds read timeout
    serset.c_cflag     |=  CREAD | CLOCAL; // turn on READ & ignore ctrl lines
    serset.c_cflag     &=  ~CRTSCTS; // no flow control

    cfmakeraw(&serset); // make raw

    tcflush( dev->get_fd(), TCIFLUSH ); // flush port
    if (tcsetattr ( dev->get_fd(), TCSANOW, &serset) != 0) // apply attributes
        return -4; // could not apply port attributes

    tcdrain(dev->get_fd());

    if(debug_output == true) cout << "succesfully connected\n";

    return 0;
}

int ser_disconnect(serdevice *dev)
{
    if(close(dev->get_fd()) < 0) return -1; // could not close device port
    if(dev->using_str())
    {
        //int stat = dev->close_stream();
        //return stat;
    }
    return 0;
}

//int serwrite(char* cmd_in, int buff_size, const char* port, bool isRS485, speed_t baud, bool togglepins, bool sendCR, bool sendLF)
string ser_IO(string cmd_in, serdevice *dev)
{
    int n, spot;
    if(dev->sendCR())
        cmd_in += '\x0D'; // append carriage return
    if(dev->sendLF())
        cmd_in += '\x0A'; // append line feed
    string result, trash;

    int buff_size = 256;
    char buffer[buff_size];
    char buf;

    //bzero(buffer,buff_size);
    //strcpy(buffer, cmd_in.c_str());

    // initialize file descriptor sets (needed for read timeout)
    fd_set read_fds, write_fds, except_fds;
    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);
    FD_ZERO(&except_fds);
    FD_SET(dev->get_fd(), &read_fds);

    // set read timeout to 5.0 seconds
    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    tcdrain(dev->get_fd());

    spot = 0;
    buf = '\0';

    // intial read
    if(dev->getpreread() && !(dev->using_str()))
    {
        bzero(buffer,buff_size);
        if (select(dev->get_fd() + 1, &read_fds, &write_fds, &except_fds, &timeout) == 1)
        {
            do
            {
                n = read( dev->get_fd(), &buf, 1 );
                sprintf( &buffer[spot], "%c", buf );
                spot += n;
            } while( buf != '\r' && buf != '\n' && buf != '\0' && n > 0 );
        }
        else
            return "!ERR6"; //"!ERROR! Read timed out.";
    }
    else if(dev->getpreread() && dev->using_str())
    {
        //trash = dev->read_stream();
    }

    //n = write(dev->get_fd(), buffer, buff_size-1);
    n = write(dev->get_fd(), cmd_in.c_str(), cmd_in.length());
    if(n < 0)
        return "!ERR4"; //"!ERROR! Write failed.";
    else if(n == 0)
        return "!ERR5"; //"!ERROR! Wrote nothing.";

    tcdrain(dev->get_fd());

    if(!(dev->using_str()))
    {
        spot = 0;
        buf = '\0';
        bzero(buffer,buff_size);

        // wait for input to become ready or until the time out
        if (select(dev->get_fd() + 1, &read_fds, &write_fds, &except_fds, &timeout) == 1)
        {
            do
            {
                n = read( dev->get_fd(), &buf, 1 );
                sprintf( &buffer[spot], "%c", buf );
                spot += n;
            } while( buf != '\r' && buf != '\n' && buf != '\0' && n > 0 );
        }
        else
            return "!ERR6"; //"!ERROR! Read timed out.";

        if (n < 0)
            return "!ERR7"; // couldn't read from port
        else if (n == 0)
            return "!ERR8"; // read nothing

        return buffer;
    }
    else
    {
        //result = dev->read_stream();
        //trash = dev->read_stream();
        //return result;
        return "finished";
    }
}

string ser_read(serdevice *dev)
{
    int buff_size = 256;
    char buffer[buff_size];
    int n, spot;
    char buf;

    // initialize file descriptor sets (needed for read timeout)
    fd_set read_fds, write_fds, except_fds;
    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);
    FD_ZERO(&except_fds);
    FD_SET(dev->get_fd(), &read_fds);

    spot = 0;
    buf = '\0';
    bzero(buffer,buff_size);

    // set read timeout to 5.0 seconds
    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    // wait for input to become ready or until the time out
    if (select(dev->get_fd() + 1, &read_fds, &write_fds, &except_fds, &timeout) == 1)
    {
        do
        {
            n = read( dev->get_fd(), &buf, 1 );
            sprintf( &buffer[spot], "%c", buf );
            spot += n;
        } while( buf != '\r' && buf != '\n' && buf != '\0' && n > 0 );
    }
    else
        return "!ERR6"; //"!ERROR! Read timed out.";

    if (n < 0)
        return "!ERR7"; // couldn't read from port
    else if (n == 0)
        return "!ERR8"; // read nothing

    return buffer;
}

string ser_write(string cmd_in, serdevice *dev)
{
    int buff_size = 256, i, n;
    char buffer[buff_size];

    bzero(buffer,buff_size);
    strcpy(buffer, cmd_in.c_str());

    for(i=0; buffer[i] != '\0';i++){} // find the end of the command within the buffer
    // add appending chars
    if(dev->sendCR() && dev->sendLF())
    {
        buffer[i] = '\r';
        buffer[i+1] = '\n';
    }
    else if(dev->sendCR())
        buffer[i] = '\r';
    else if(dev->sendLF())
        buffer[i] = '\n';

    tcdrain(dev->get_fd());

    n = write(dev->get_fd(), buffer, buff_size-1);
    if(n < 0)
        return "!ERR4"; //"!ERROR! Write failed.";
    else if(n == 0)
        return "!ERR5"; //"!ERROR! Wrote nothing.";

    tcdrain(dev->get_fd());

    return "DONE";
}

int ser_latch(serdevice *dev)
{
    int buff_size = 256;
    char buffer[buff_size];
    bool hold = true;
    int n, i, spot;
    char buf;

    // initialize file descriptor sets (needed for read timeout)
    fd_set read_fds, write_fds, except_fds;
    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);
    FD_ZERO(&except_fds);
    FD_SET(dev->get_fd(), &read_fds);

    // set read timeout to 5.0 seconds
    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    while(hold)
    {
        //unsigned char cmd[cmd_st.length()+1];
        //strcpy((char*)cmd, cmd_in);
        bzero(buffer,buff_size);
        char* useless = fgets(buffer,(buff_size-1),stdin); // take user input and write to buffer
        free(useless);

        if(buffer[0] != 'x' && buffer[0] != 'X') // check for escape keys
        {
            for(i=0; buffer[i] != '\0';i++){} // find the end of the command within the buffer
            // add appending chars
            if(dev->sendCR() && dev->sendLF())
            {
                buffer[i] = '\r';
                buffer[i+1] = '\n';
            }
            else if(dev->sendCR())
                buffer[i] = '\r';
            else if(dev->sendLF())
                buffer[i] = '\n';

            n = write(dev->get_fd(), buffer, buff_size-1);
            if(n < 0)
                return -4; //"!ERROR! Write failed.";
            else if(n == 0)
                return -5; //"!ERROR! Wrote nothing.";

            tcdrain(dev->get_fd());

            spot = 0;
            buf = '\0';
            bzero(buffer,buff_size);

            // wait for input to become ready or until the time out
            if (select(dev->get_fd() + 1, &read_fds, &write_fds, &except_fds, &timeout) == 1)
            {
                do
                {
                    n = read( dev->get_fd(), &buf, 1 );
                    sprintf( &buffer[spot], "%c", buf );
                    spot += n;
                } while( buf != '\r' && buf != '\n' && buf != '\0' && n > 0 );
            }
            else
                return -6; //"!ERROR! Read timed out.";

            if (n < 0)
                return -7; // couldn't read from port
            else if (n == 0)
                return -8; // read nothing
            else
                printf("%.*s", n, buffer);
        }
        else hold = false;
    }
    return 0;
}

// execute a system command and return the output in a string
string exec_cmd(const char* cmd)
{
    int buff_size = 256;
    char buffer[buff_size];
    string result = "";
    FILE* pipe = popen(cmd, "r");
    if (!pipe) throw runtime_error("popen() failed!");
    try {
        while (!feof(pipe)) {
            if (fgets(buffer, buff_size, pipe) != NULL)
                result += buffer;
        }
    } catch (...) {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    return result;
}
