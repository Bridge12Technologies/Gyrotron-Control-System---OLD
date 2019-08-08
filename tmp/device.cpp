#include "device.h"

Device::Device() {}

Device::Device(std::string nickname) 
{ load_from_config(nickname); }

Device::Device(std::string nickname, std::string name)
{ load_from_config(nickname); dev_name = name; }

std::string Device::get_name() { return dev_name; }

void Device::set_name(std::string name) { dev_name = name; }

std::mutex* Device::m() { return &mu; }
std::mutex* Device::warning_m() { return &warning_mu; }
std::mutex* Device::error_m() { return &error_mu; }

void Device::push_warning(std::string warn) { warnings.push_back(warn); }
void Device::clear_warnings() { warnings.clear(); }
std::vector<std::string> Device::get_warnings() { return warnings;}

void Device::m_push_warning(std::string warn)
{ std::lock_guard<std::mutex> lock(warning_mu); warnings.push_back(warn); }

void Device::m_clear_warnings()
{ std::lock_guard<std::mutex> lock(warning_mu); warnings.clear(); }

std::vector<std::string> Device::m_get_warnings()
{ std::lock_guard<std::mutex> lock(warning_mu); return warnings; }

void Device::push_error(std::string err) { errors.push_back(err); }
void Device::clear_errors() { errors.clear(); }
std::vector<std::string> Device::get_errors() { return errors;}

void Device::m_push_error(std::string err)
{ std::lock_guard<std::mutex> lock(error_mu); errors.push_back(err); }

void Device::m_clear_errors()
{ std::lock_guard<std::mutex> lock(error_mu); errors.clear(); }

std::vector<std::string> Device::m_get_errors()
{ std::lock_guard<std::mutex> lock(error_mu); return errors; }

void Device::set_com_port(std::string com) { com_port = com; }

void Device::set_ip(std::string ip) { hostname = ip; }

void Device::set_init_pause(double duration) { init_pause_secs = duration; }

void Device::set_io_pause(double duration) { io_pause_secs = duration; }

void Device::set_enabled(bool enable) { dev_enabled = enable; }

bool Device::is_enabled() { return dev_enabled; }

bool Device::is_connected() { return dev_connected; }

int Device::connect()
{
    int stat;

    if(dev_type == 'E')
        stat = eth_connect();
    else
        stat = ser_connect();

    if(needs_init_pause)
        usleep(__useconds_t(init_pause_secs * 1000000.00));

    if(stat == 0)
        dev_connected = true;

    return stat;
}

int Device::disconnect()
{
    dev_connected = false;

    if(dev_type == 'E')
        return eth_disconnect();
    return ser_disconnect();
}

int Device::load_from_config(std::string nickname)
{
    bool found = false;
    std::string line, substr;
    std::stringstream ss;
    std::ifstream file("device_config.csv");
    std::getline(file,line); // ignore first line

    while(std::getline(file, line) && !found)
    {
        if(line.substr(0,line.find(",")) == nickname) // if first entry of line matches provided name
        {
            found = true;
            clean_str(line);
            line += ",";
            //std::cout << line << "\n";
            ss.str(line);
            std::getline(ss, substr, ',');
            dev_name = substr;
            std::getline(ss, substr, ','); // skip long name
            std::getline(ss, substr, ',');
            dev_type = substr[0];
            std::getline(ss, substr, ',');
            prepend_str = substr;
            std::getline(ss, substr, ',');
            append_str = substr;
            std::getline(ss, substr, ',');
            if(substr == "yes")
                needs_preread = true;
            else
                needs_preread = false;
            std::getline(ss, substr, ',');
            if(substr == "yes")
                needs_postread = true;
            else
                needs_postread = false;
            std::getline(ss, substr, ',');
            if(substr == "yes")
                needs_io_pause = true;
            else
                needs_io_pause = false;
            std::getline(ss, substr, ',');
            if(substr == "yes")
                needs_init_pause = true;
            else
                needs_init_pause = false;
            std::getline(ss, substr, ',');
            if(dev_type == 'E')
            {
                hostname = substr;
                std::getline(ss, substr, ',');
                portnum = std::stoi(substr);
            }
            else
            {
                com_port = substr;
                std::getline(ss, substr, ',');
                if(substr == "9600")
                    baud = B9600;
                else if(substr == "19200")
                    baud = B19200;
                else if(substr == "38400")
                    baud = B38400;
                else if(substr == "57600")
                    baud = B57600;
                else if(substr == "115200")
                    baud = B115200;
            }
            return 0;
        }
    }

    return -1;
}

void Device::print_config()
{
    std::cout << "Name: " << dev_name << "\n";
    std::cout << "Type: " << dev_type << "\n";
    std::cout << "Prepend: \"" << prepend_str << "\"\n";
    std::cout << "Append: \"" << append_str << "\"\n";
    std::cout << "Pre-read?: " << needs_preread << "\n";
    std::cout << "Post-read?: " << needs_postread << "\n";
    std::cout << "IO Pause?: " << needs_io_pause << "\n";
    std::cout << "Init Pause?: " << needs_init_pause << "\n";
    if(dev_type == 'E')
    {
        std::cout << "Hostname: " << hostname << "\n";
        std::cout << "Port: " << portnum << "\n";
    }
    else
    {
        std::cout << "COM Port: " << com_port << "\n";
        std::cout << "Baud: ";
        if(baud == B9600)
            std::cout << "9600\n";
        else if(baud == B19200)
            std::cout << "19200\n";
        else if(baud == B38400)
            std::cout << "38400\n";
        else if(baud == B57600)
            std::cout << "57600\n";
        else if(baud == B115200)
            std::cout << "115200\n";
    }
}

std::string Device::io(std::string cmd_in)
{
    if(dev_type == 'E')
        return eth_io(cmd_in);
    return ser_io(cmd_in);
}

std::string Device::m_io(std::string cmd_in)
{
    std::lock_guard<std::mutex> lock(mu);
    return io(cmd_in);
}

std::string Device::smart_io(std::string cmd, std::string expect, bool does_contain)
{
    int tries = 0;
    std::string resp;
    while(tries < 3)
    {
        resp = m_io(cmd);
        if(expect == "")
        {
            if(resp.find("ERR") == std::string::npos)
                return resp;
        }
        else if(does_contain)
        {
            if(resp.find(expect) != std::string::npos && resp.find("ERR") == std::string::npos)
                return resp;
        }
        else
        {
            if(resp.find(expect) == std::string::npos && resp.find("ERR") == std::string::npos)
                return resp;
        }
        usleep(100000); // wait 0.1 sec to retry
        tries++;
    }
    disconnect();
    return "!ERR-DC";
}

std::string Device::smart_write(std::string cmd1, std::string cmd2, std::string expect, bool does_contain)
{
    int tries = 0;
    std::string resp;
    while(tries < 3)
    {
        m_write(cmd1);
        usleep(100000); // 0.1s delay before checking if cmd was properly applied
        resp = m_io(cmd2);
        if(expect == "")
        {
            if(resp.find("ERR") == std::string::npos)
                return resp;
        }
        else if(does_contain)
        {
            if(resp.find(expect) != std::string::npos && resp.find("ERR") == std::string::npos)
                return resp;
        }
        else
        {
            if(resp.find(expect) == std::string::npos && resp.find("ERR") == std::string::npos)
                return resp;
        }
        usleep(100000); // wait 0.1 sec to retry
        tries++;
    }
    disconnect();
    return "!ERR-DC";
}

std::string Device::smart_read(std::string expect, bool does_contain)
{
    int tries = 0;
    std::string resp;
    while(tries < 3)
    {
        resp = m_read();
        if(expect == "")
        {
            if(resp.find("ERR") == std::string::npos)
                return resp;
        }
        else if(does_contain)
        {
            if(resp.find(expect) != std::string::npos && resp.find("ERR") == std::string::npos)
                return resp;
        }
        else
        {
            if(resp.find(expect) == std::string::npos && resp.find("ERR") == std::string::npos)
                return resp;
        }
        usleep(100000); // wait 0.1 sec to retry
        tries++;
    }
    disconnect();
    return "!ERR-DC";
}

std::string Device::read()
{
    if(dev_type == 'E')
        return eth_read();
    return ser_read();
}

std::string Device::m_read()
{
    std::lock_guard<std::mutex> lock(mu);
    return read();
}

int Device::write(std::string cmd_in)
{
    if(dev_type == 'E')
        return eth_write(cmd_in);
    return ser_write(cmd_in);
}

int Device::m_write(std::string cmd_in)
{
    std::lock_guard<std::mutex> lock(mu);
    return write(cmd_in);
}

int Device::probe(std::string test_cmd, std::string expected)
{
    std::string resp = io(test_cmd);

    if(resp.find(expected) == std::string::npos)
    {
        return -1;
    }

    if(debug_output)
        std::cout << "probe response: " << resp << "\n";

    return 0;
}

void Device::clean_str(std::string& str)
{
    str.erase(std::remove(str.begin(), str.end(), '\x02'), str.end()); // erase STX
    str.erase(std::remove(str.begin(), str.end(), '\x03'), str.end()); // erase ETX
    str.erase(std::remove(str.begin(), str.end(), '\x0D'), str.end()); // erase CR
    str.erase(std::remove(str.begin(), str.end(), '\x0A'), str.end()); // erase LF
}

void Device::prep_str(std::string& str)
{
    clean_str(str);
    if(prepend_str.find("\\stx") != std::string::npos || prepend_str.find("\\x02") != std::string::npos)
        str = '\x02' + str;
    if(append_str.find("\\r") != std::string::npos || append_str.find("\\x0D") != std::string::npos)
        str += '\x0D';
    if(append_str.find("\\n") != std::string::npos || append_str.find("\\x0A") != std::string::npos)
        str += '\x0A';
    if(append_str.find("\\etx") != std::string::npos || append_str.find("\\x03") != std::string::npos)
        str += '\x03';
}

int Device::eth_connect()
{
    struct sockaddr_in serv_addr; // device address
    struct hostent *server; // pointer to host object

    if(debug_output) std::cout << "opening socket for " << dev_name << "...\n";

    // open socket
    dev_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (dev_socket < 0) return -1; // couldn't open socket

    struct timeval tv;
    tv.tv_sec = 3; // timeout in seconds
    tv.tv_usec = 0;
    setsockopt(dev_socket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&tv), sizeof tv); // apply timeout settings

    if(debug_output == true)
    {
        std::cout << "using socket " << dev_socket << std::endl;
        std::cout << "getting host name...\n";
    }

    // parse host name
    if(debug_output) std::cout << "hostname:" << hostname.c_str() << "\n";
    server = gethostbyname(hostname.c_str());
    if (server == nullptr) return -2; // host not found

    // zero the server address array and parse
    bzero(reinterpret_cast<char *>(&serv_addr), sizeof(serv_addr));
    //memset((char *) &serv_addr, 0, sizeof(serv_addr)); // doesn't work for some reason
    serv_addr.sin_family = AF_INET;
    bcopy(reinterpret_cast<char *>(server->h_addr), reinterpret_cast<char *>(&serv_addr.sin_addr.s_addr), size_t(server->h_length));
    //memmove((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length); // doesn't work for some reason

    if(debug_output) std::cout << "connecting to server...\n";

    // connect to server
    serv_addr.sin_port = htons(uint16_t(portnum));
    if(debug_output)
    {
        std::cout << "port (host):" << portnum << std::endl;
        std::cout << "port (net):" << serv_addr.sin_port << std::endl;
    }
    if (::connect(dev_socket,reinterpret_cast<struct sockaddr*>(&serv_addr),sizeof(serv_addr)) < 0)
    {
        std::cout << strerror(errno) << "\n";
        return -3; // couldn't connect
    }

    if(debug_output) std::cout << "done setting up socket.\n";
    return 0;
}

int Device::eth_disconnect()
{
    if(close(dev_socket) < 0) return -1; // close socket
    return 0;
}

std::string Device::eth_io(std::string cmd_in)
{
    size_t buff_size = 256;
    char buffer[256];
    int n; // file descriptor, number of chars, tracking int

    prep_str(cmd_in);

    if(debug_output == true) std::cout << "pointed to " << dev_name << std::endl;

    // intial read
    if(needs_preread)
    {
        if(debug_output == true) std::cout << "performing initial read...";
	    memset(buffer, 0, buff_size);
        n = int(recv(dev_socket, &buffer[0], (buff_size-1), 0));
        if (n < 0) return "!ERR4"; // couldn't read socket
	}

    memset(buffer, 0, buff_size);
    strcpy(buffer,cmd_in.c_str());

    // write command to socket
    if(debug_output == true) std::cout << "writing:" << buffer << std::endl;
    n = int(send(dev_socket, &buffer[0], strlen(buffer), 0)); //MSG_NOSIGNAL
    if(debug_output == true) std::cout << "send return #:" << n << std::endl;
    if (n < 0) return "!ERR5"; // couldn't write to socket

    if(needs_io_pause)
        usleep(__useconds_t(io_pause_secs * 1000000.00));

    if(debug_output == true) std::cout << "reading...\n";

    // zero the buffer and read from the socket
    memset(buffer, 0, buff_size);
    n = int(recv(dev_socket, &buffer[0], (buff_size-1), 0));
    if (n < 0) return "!ERR4"; // couldn't read socket
    if(debug_output == true) std::cout << "read: " << buffer << std::endl;

    std::string return_str = buffer;
    clean_str(return_str);

    return return_str;
}

std::string Device::eth_read()
{
    size_t buff_size = 256;
    char buffer[256];
    int n; // file descriptor, number of chars, tracking int

    if(debug_output == true)
    {
        std::cout << "pointed to " << dev_name << std::endl;
        std::cout << "reading...\n";
    }

    // zero the buffer and read from the socket
    memset(buffer, 0, buff_size);
    n = int(recv(dev_socket, &buffer[0], (buff_size-1), 0));
    if (n < 0) return "!ERR4"; // couldn't read socket
    if(debug_output == true) std::cout << "read: " << buffer << std::endl;

    std::string return_str = buffer;
    clean_str(return_str);

    return return_str;
}

int Device::eth_write(std::string cmd_in)
{
    bool debug_output = false;

    size_t buff_size = 256;
    char buffer[256];
    int n; // file descriptor, number of chars, tracking int

    prep_str(cmd_in);

    if(debug_output == true)
        std::cout << "pointed to " << dev_name << std::endl;

    memset(buffer, 0, buff_size);
    strcpy(buffer,cmd_in.c_str());

    // write command to socket
    if(debug_output == true)
        std::cout << "writing:" << buffer << std::endl;
    n = int(send(dev_socket, &buffer[0], strlen(buffer), MSG_NOSIGNAL));
    if (n < 0)
        return -1; // couldn't write to socket
    if(debug_output == true)
        std::cout << "done writing.\n";

    return 0;
}

int Device::ser_connect()
{
    if(debug_output == true) std::cout << "connecting serially to " << dev_name << std::endl;

    //struct termios sset_send, sset_recv;
    struct termios serset;// serset_prev;
    memset (&serset, 0, sizeof serset);

    // open the port
    //int fd = open( port.c_str(), O_RDWR| O_NOCTTY );
    fd = open(com_port.c_str(), O_RDWR| O_NOCTTY);
    if(fd < 0)
        return -1; // failed to open port

    // initialize file descriptor sets (needed for read timeout)
    fd_set read_fds, write_fds, except_fds;
    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);
    FD_ZERO(&except_fds);
    FD_SET(fd, &read_fds);

    // load existing port attributes
    if (tcgetattr (fd, &serset) != 0)
        return -3; // failed to load existing port attributes

    // set baud rate
    cfsetospeed (&serset, baud);
    cfsetispeed (&serset, baud);

    // set various port attributes
    serset.c_cflag     &=  unsigned(~PARENB); // make 8n1
    serset.c_cflag     &=  unsigned(~CSTOPB);
    serset.c_cflag     &=  unsigned(~CSIZE);
    serset.c_cflag     |=  CS8;
    serset.c_cc[VMIN]   =  1; // previously 1
    serset.c_cc[VTIME]  =  5; // 0.5 seconds read timeout
    serset.c_cflag     |=  CREAD | CLOCAL; // turn on READ & ignore ctrl lines
    serset.c_cflag     &=  ~CRTSCTS; // no flow control

    cfmakeraw(&serset); // make raw

    tcflush( fd, TCIFLUSH ); // flush port
    if (tcsetattr ( fd, TCSANOW, &serset) != 0) // apply attributes
        return -4; // could not apply port attributes

    tcdrain(fd);

    if(debug_output == true) std::cout << "succesfully connected\n";
    return 0;
}

int Device::ser_disconnect()
{
    if(close(fd) < 0) return -1; // could not close device port
    return 0;
}

std::string Device::ser_io(std::string cmd_in)
{
    int n, spot;
    std::string result = "";
    size_t buff_size = 256;
    char buffer[256];
    char buf;

    prep_str(cmd_in);

    // initialize file descriptor sets (needed for read timeout)
    fd_set read_fds, write_fds, except_fds;
    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);
    FD_ZERO(&except_fds);
    FD_SET(fd, &read_fds);

    // set read timeout to 5.0 seconds
    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    tcdrain(fd);

    spot = 0;
    buf = '\0';

    // intial read
    if(needs_preread)
    {
        memset(buffer, 0, buff_size);
        if (select(fd + 1, &read_fds, &write_fds, &except_fds, &timeout) == 1)
        {
            do
            {
                n = int(::read( fd, &buf, 1 ));
                sprintf( &buffer[spot], "%c", buf );
                spot += n;
            } while( buf != '\r' && buf != '\n' && buf != '\0' && n > 0 );
        }
        else
            return "!ERR6"; //"!ERROR! Read timed out.";
    }

    //n = write(fd, buffer, buff_size-1);
    n = int(::write(fd, cmd_in.c_str(), cmd_in.length()));
    if(n < 0)
        return "!ERR4"; //"!ERROR! Write failed.";
    else if(n == 0)
        return "!ERR5"; //"!ERROR! Wrote nothing.";

    tcdrain(fd);

    if(needs_io_pause)
        usleep(__useconds_t(io_pause_secs * 1000000.00));

    spot = 0;
    buf = '\0';
    memset(buffer, 0, buff_size);

    // wait for input to become ready or until the time out
    if (select(fd + 1, &read_fds, &write_fds, &except_fds, &timeout) == 1)
    {
        do
        {
                n = int(::read( fd, &buf, 1 ));
                //sprintf( &buffer[spot], "%c", buf );
                result += buf;
                spot += n;
        }
        while( buf != '\r' && buf != '\n' && buf != '\0' && n > 0 );
    }
    else
        return "!ERR6"; //"!ERROR! Read timed out.";

    if (n < 0)
        return "!ERR7"; // couldn't read from port
    else if (n == 0)
        return "!ERR8"; // read nothing

    //result = buffer; // store the actual response here to be returned

    if(needs_postread)
    {
        spot = 0;
        buf = '\0';
        memset(buffer, 0, buff_size);

        // wait for input to become ready or until the time out
        if (select(fd + 1, &read_fds, &write_fds, &except_fds, &timeout) == 1)
        {
            do
            {
                n = int(::read( fd, &buf, 1 ));
                sprintf( &buffer[spot], "%c", buf );
                spot += n;
            }
            while( buf != '\r' && buf != '\n' && buf != '\0' && n > 0 );
        }
        else
            return "!ERR9"; //"!ERROR! Read timed out.";
    }

    clean_str(result);

    return result;
}

std::string Device::ser_read()
{
    size_t buff_size = 256;
    char buffer[256];
    int n, spot;
    char buf;

    // initialize file descriptor sets (needed for read timeout)
    fd_set read_fds, write_fds, except_fds;
    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);
    FD_ZERO(&except_fds);
    FD_SET(fd, &read_fds);

    spot = 0;
    buf = '\0';
    memset(buffer, 0, buff_size);

    // set read timeout to 5.0 seconds
    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    // wait for input to become ready or until the time out
    if (select(fd + 1, &read_fds, &write_fds, &except_fds, &timeout) == 1)
    {
        do
        {
            n = int(::read( fd, &buf, 1 ));
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

    std::string return_str = buffer;
    clean_str(return_str);

    return return_str;
}

int Device::ser_write(std::string cmd_in)
{
    int n;

    prep_str(cmd_in);

    tcdrain(fd);

    n = int(::write(fd, cmd_in.c_str(), cmd_in.length()));
    if(n <= 0)
        return -1;

    tcdrain(fd);

    return 0;
}
