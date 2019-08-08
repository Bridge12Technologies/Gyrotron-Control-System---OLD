#ifndef DEVICE_H
#define DEVICE_H

#include <termios.h> // POSIX terminal control definitions
#include <fstream> // file streaming
#include <unistd.h> // UNIX standard function definitions (read/write/usleep/select/etc)
#include <fcntl.h> // file control definitions
#include <netdb.h> // used for ethernet communication
#include <string>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <mutex>
#include <atomic>
#include <vector>

class Device
{
public:
    Device(); // allows initializing blank device to be manually configured
    Device(std::string nickname); // this will load configuration of the given nickname from config file
    Device(std::string nickname, std::string name); // same here, but also allows overriding the device name field
    int load_from_config(std::string nickname); // extract device settings from device_config.csv
    void print_config(); // print device configuration via cout, 10 lines total
    std::string get_name(); 
    void set_name(std::string name);
    int connect();
    int disconnect();
    void set_enabled(bool enable); // toggle the "enabled" flag, which doesn't do anything but is a useful indicator
    bool is_enabled();
    bool is_connected(); // returns true is successful connection has been made with device
    int probe(std::string test_cmd, std::string expected); // write test_cmd and check if read contains expected substring
    std::string io(std::string cmd_in); // write then read
    std::string m_io(std::string cmd_in); // thread-safe write then read
    std::string read();
    std::string m_read(); // thread-safe
    int write(std::string cmd_in);
    int m_write(std::string cmd_in); // thread-safe
    void set_com_port(std::string com);
    void set_ip(std::string ip);
    void set_init_pause(double duration); // set pause duration following successful connection (sec)
    void set_io_pause(double duration); // set pause duration in between write and read (sec)
    std::mutex* m(); // return pointer to device mutex

    std::mutex* warning_m(); // return pointer to faults mutex
    void push_warning(std::string fault);
    std::vector<std::string> get_warnings();
    void clear_warnings();
    void m_push_warning(std::string fault);
    std::vector<std::string> m_get_warnings();
    void m_clear_warnings();

    std::mutex* error_m(); // return pointer to faults mutex
    void push_error(std::string fault);
    std::vector<std::string> get_errors();
    void clear_errors();
    void m_push_error(std::string fault);
    std::vector<std::string> m_get_errors();
    void m_clear_errors();

    // these special i/o fucntions will check output and try again if failed
    std::string smart_io(std::string cmd, std::string expect = "", bool does_contain = true);
    std::string smart_write(std::string cmd1, std::string cmd2, std::string expect = "", bool does_contain = true);
    std::string smart_read(std::string expect = "", bool does_contain = true);

    bool operator == (Device& other) // equality operator will compare hostname or com_port
    {
        if(dev_type == 'E')
            return hostname == other.hostname;
        else if(dev_type == 'S')
            return com_port == other.com_port;
        return false;
    }

private:
    // ethernet specific functions
    int eth_connect();
    int eth_disconnect();
    std::string eth_io(std::string cmd_in);
    std::string eth_read();
    int eth_write(std::string cmd_in);

    // serial specific functions
    int ser_connect();
    int ser_disconnect();
    std::string ser_io(std::string cmd_in);
    std::string ser_read();
    int ser_write(std::string cmd_in);

    void clean_str(std::string& str); // remove LF, CR, STX, and ETX
    void prep_str(std::string& str); // format with assigned prepending and appending chars

    bool debug_output{false};

    std::string dev_name;
    char dev_type;
    std::string prepend_str, append_str;
    bool needs_preread{false}, needs_postread{false};
    bool needs_io_pause{false}, needs_init_pause{false};
    double init_pause_secs{2}, io_pause_secs{0.5};
    std::mutex mu, warning_mu, error_mu;
    std::atomic<bool> dev_enabled{true}, dev_connected{false};
    std::vector<std::string> warnings, errors;

    std::string com_port;
    speed_t baud;
    int fd;

    int portnum, dev_socket;
    std::string hostname;
};

#endif
