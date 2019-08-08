#include <bits/stdc++.h> // for converting to lowercase
#include <map>
#include <sstream> // for convering sci string to double
#include <functional> // function pointers/args
#include <chrono> // time library
#include <cstddef> // for std::nullptr_t type
#include "ctpl_stl.h" // thread pool
#include "logger.h" // logging
#include "device.h" // device I/O

using clk = std::chrono::high_resolution_clock;
typedef std::function<void()> F;

// for simplicity, pass std::bind(&Object::function, this) for function args
// lambda functions which capture the object should also work

double to_doub(std::string str); // alternative to std::stod with error checking, returns -1 for failure
bool approx_equal(double a, double b); // check for equality within tolerance of 1e-6
void remove_all(std::string &str, std::string sub); // remove all occurances of sub within str
bool isOK(std::string str); // true if string contains "OK"
bool err(std::string str); // true if string contains "ERR"
bool contains(std::string str, std::string sub); // true if str contains sub
bool starts_with(std::string str, std::string sub); // true if str begins with sub
std::string to_str(auto &num); // just a shorter wrapper for std::to_string
std::string dec2hex(int num); // convert decimal integer value into hex string
int hex2dec(std::string str); // convert hex string into decimal integer
std::string hex2bin(std::string str); // convert hex string into binary string
std::string sci_str(double val, int prec = 3); // format double as string in scientific notation

class Spine
{
public:
    int init(); // spawn master thread, connect and probe devices
    void stop(); // end query loop and kill master thread
    void log_event(std::string str);
    void log_data(std::string str);
    std::string get_event_history();
    std::string get_data_history();
    std::vector<std::string> get_sys_errors();
    std::vector<std::string> get_sys_warnings();

protected:
    Spine(); // default constructor with both logs enabled
    Spine(bool alog_on, bool dlog_on);
    // add device accepts device pointer, query func, reconnection func,
    // probe command, expected probe response, query frequency (default 0s),
    // and reconnect attempt frequency (default 1min or 600s)
    void add_device(Device *dev, F qfunc, F rfunc, std::string pcmd, 
                    std::string presp, double qfreq = 0, double rfreq = 600);
    void add_pre_func(F func); // add function to master thread pre-loop sequence
    void add_loop_func(F func); // add function to master thread loop sequence (repeated)
    void add_post_func(F func); // add function to master thread pre-loop sequence
    void exec_once(F func); // add function to master thread for single execution, followed by deletion
    std::string shout(std::string str); // thread-safe console output
    int parse_config(); // parse configuration file into string map
    double config_get_double(std::string title, bool &ok); // retrieve double value for key containing title
    bool config_get_bool(std::string title, bool &ok); // retrieve bool value for key containing title
    std::string config_get_str(std::string title, bool &ok); // retrieve string value for key containing title
    bool config_entry_empty(std::string title, bool &ok); // check if value field is empty for key containing title
    void set_alog_header(std::string header);
    void set_dlog_header(std::string header);

    Log elog, dlog;
    std::mutex sys_error_m, sys_warning_m;
    std::vector<std::string> sys_errors, sys_warnings;

private:
    void master_task_loop(std::promise<int> * init_stat_promise); // main thread which manages logs and thread pool
    int connect_and_probe(); // connect to and prove all devices in list
    void query_devices(ctpl::thread_pool& tpool); // will ignore disabled devices
    void exec_loop_funcs(ctpl::thread_pool& tpool);
    void disconnect_all();
    void exec_pre_funcs(); // execute pre-loop functions serially
    void exec_loop_funcs(); // execute recurring functions serially
    void exec_single_funcs(); // execute single-use functions serially
    void exec_post_funcs(); // execute post-loop functions serially

    // thread-safe get/set functions
    bool is_available(int index);
    void set_available(int index, bool state);
    double get_query_freq(int index);
    clk::time_point get_last_query(int index);
    void set_last_query(int index, clk::time_point time);
    std::string get_probe_cmd(int index);
    std::string get_probe_resp(int index);

    std::thread master_thread;
    std::atomic<bool> master_thread_active{true};
    std::atomic<int> fault_state{0};
    
    std::chrono::duration<double, std::milli> query_elapsed;
    double elapsed_secs;

    bool alog_enabled{true}, dlog_enabled{true};

    std::map<std::string,std::string> config_map;

    std::mutex start_time_m;
    std::mutex available_m, pre_m, recon_m, loop_m, post_m, single_m;
    std::mutex query_m, devs_m, probe_cmd_m, probe_resp_m;
    std::mutex last_query_m, last_recon_m, query_freq_m, recon_attempt_m;

    std::vector<bool> devs_available;
    std::vector<F> pre_funcs, loop_funcs, post_funcs, single_funcs;
    std::vector<F> query_funcs, recon_funcs;
    std::vector<Device*> devices;
    std::vector<std::string> probe_cmds, probe_resps;
    std::vector<clk::time_point> last_queries, last_recons;
    std::vector<double> query_freqs, recon_attempt_freqs;
};