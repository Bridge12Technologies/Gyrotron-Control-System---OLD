#include <string>
#include <mutex>
#include <ctime>
#include <fstream>
#include <sys/stat.h>

class Log
{
public:
    Log();
    Log(bool is_data);
    void create_new(); // create/open log file and add header
    void close(); // end log and close file
    void set_enabled(bool enable); // enable/disable write/begin function
    void write(std::string log_entry, bool prepend_time = true); // add log entry
    int num_logs(); // get number of log files created
    bool is_enabled(); // true if log is enabled
    long get_max_size(); // get maximum allowed log file size
    void set_max_size(long new_max); // set max file size
    void set_header(std::string str); // set the log header before create_new
    void set_history_enabled(bool enable); // enabled log history function
    std::string get_history(); // retrieve and clear log history string

private:
    long get_file_size(std::string file);

    int num_log{0};
    long max_file_size{1073741824};
    bool log_enabled{true}, data_format{false}, history_enabled{false};
    std::string filename, header{""};
    std::mutex m, history_m;
    std::fstream log_stream;
    std::string log_history{""};
};