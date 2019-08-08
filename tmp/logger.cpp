#include "logger.h"

Log::Log() {}

Log::Log(bool is_data) : data_format{is_data} {}

void Log::create_new()
{
    if(log_enabled)
    {
        std::string out_str, filename_temp;
        char timestamp_full[25], timestamp_file[20]; // tweak sizes later if necessary
        time_t t = time(nullptr);
        strftime(timestamp_full, sizeof(timestamp_full), "%R on %m/%d/%Y", localtime(&t));
        strftime(timestamp_file, sizeof(timestamp_file), "%Y-%m-%d_%H-%M", localtime(&t));
        
        if(num_log > 0)
            filename_temp = filename;

        if(data_format)
            filename = "data/" + std::string(timestamp_file) + ".dat";
        else
            filename = "logs/" + std::string(timestamp_file) + ".log";

        if(num_log > 0)
        {
            write("# FILE BREAK - continues in \"" + filename + "\"\n");
            log_stream.close();
        }

        log_stream.open(filename.c_str(), std::ios::out | std::ios::app);

        if(num_log == 0)
            out_str = "# BEGIN LOG at " + std::string(timestamp_full);
        else
            out_str = "# CONTINUE LOG at " + std::string(timestamp_full) + ", previous log file: \"" + filename_temp + "\"";

        if(header != "")
            out_str += "\n# " + header;

        write(out_str, false);
        num_log++;
    }
}

void Log::close()
{
    if(!data_format)
        write("closed program");
    if(log_stream.is_open())
        log_stream.close();
}

void Log::set_header(std::string str) { header = str; }

void Log::set_enabled(bool enable) { log_enabled = enable; }

void Log::write(std::string log_entry, bool prepend_time)
{
    if(log_enabled)
    {
        if(prepend_time)
        {
            char timestamp[10];
            time_t t = time(nullptr);
            struct tm * now = localtime(&t);
            strftime(timestamp, sizeof(timestamp), "%T", now);
            if(data_format)
                log_entry = std::string(timestamp) + "," + log_entry;
            else
                log_entry = std::string(timestamp) + " - " + log_entry;
        }

        if(history_enabled)
        {
            history_m.lock();
            log_history += log_entry + "\n";
            history_m.unlock();
        }

        m.lock();
        log_stream << log_entry << "\n";
        log_stream.flush();
        if(get_file_size(filename) > max_file_size) // check file size and break if too large
            create_new();
        m.unlock();
    }
}

int Log::num_logs() { return num_log; }

bool Log::is_enabled() { return log_enabled; }

long Log::get_max_size() { return max_file_size; }

void Log::set_max_size(long new_max) { max_file_size = new_max; }

long Log::get_file_size(std::string file) // query the size of a given file
{
    struct stat stat_buf;
    int rc = stat(file.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

void Log::set_history_enabled(bool enable) { history_enabled = enable; }

std::string Log::get_history()
{ 
    std::lock_guard<std::mutex> lock(history_m);
    std::string ret_str = log_history;
    log_history = "";
    return ret_str;
}