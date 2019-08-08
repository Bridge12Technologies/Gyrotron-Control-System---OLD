#include "spine.h"

 // NOTE: might need references or pointers for function args

Spine::Spine() {} // default w/ both logs enabled

Spine::Spine(bool alog_on, bool dlog_on) : alog_enabled(alog_on), dlog_enabled(dlog_on) {}

int Spine::init()
{
    std::promise<int> init_stat_promise;
    std::future<int> init_stat_future = init_stat_promise.get_future();
    master_thread = std::thread(&Spine::master_task_loop, this, &init_stat_promise);
    return init_stat_future.get();
}

void Spine::stop()
{ master_thread_active = false; master_thread.join(); }

void Spine::set_alog_header(std::string header)
{ alog.set_header(header); }

void Spine::set_dlog_header(std::string header)
{ dat.set_header(header); }

void Spine::add_device(Device *dev, F qfunc, F rfunc, std::string pcmd,
                       std::string presp, double qfreq, double rfreq)
{
    devs_m.lock(); devices.push_back(dev); devs_m.unlock(); 
    query_m.lock(); query_funcs.push_back(qfunc); query_m.unlock();
    recon_m.lock(); recon_funcs.push_back(rfunc); recon_m.unlock();
    probe_cmd_m.lock(); probe_cmds.push_back(pcmd); probe_cmd_m.unlock();
    probe_resp_m.lock(); probe_resps.push_back(presp); probe_resp_m.unlock();
    last_query_m.lock(); last_queries.push_back(clk::now()); last_query_m.unlock();
    last_recon_m.lock(); last_recons.push_back(clk::now()); last_recon_m.unlock();
    query_freq_m.lock(); query_freqs.push_back(qfreq); query_freq_m.unlock();
    recon_attempt_m.lock(); recon_attempt_freqs.push_back(rfreq); recon_attempt_m.unlock();
    available_m.lock(); devs_available.push_back(true); available_m.unlock();
}

void Spine::add_loop_func(F func)
{ std::lock_guard<std::mutex> lock(loop_m); loop_funcs.push_back(func); }

void Spine::add_pre_func(F func)
{ std::lock_guard<std::mutex> lock(pre_m); pre_funcs.push_back(func); }

void Spine::add_post_func(F func)
{ std::lock_guard<std::mutex> lock(post_m); post_funcs.push_back(func); }

void Spine::exec_once(F func) 
{ std::lock_guard<std::mutex> lock(single_m); single_funcs.push_back(func); }

void Spine::master_task_loop(std::promise<int> * init_stat_promise)
{
    if(alog_enabled) alog.create_new();
    if(dlog_enabled) dat.create_new();

    int init_stat = connect_and_probe(); // device initialization
    init_stat_promise->set_value(init_stat);

    if(init_stat == 0) // continue only if device initialization is successful
    {
        // create thread pool with # of threads = max possible concurrent threads
        ctpl::thread_pool tpool((int(std::thread::hardware_concurrency())));

        exec_pre_funcs(); // execute pre-loop functions serially

        while(master_thread_active) // continue this loop until stop()
        {
            query_devices(tpool); // push device query functions to thread pool
            exec_loop_funcs(); // execute recurring functions serially
            exec_single_funcs(); // execute single-use functions serially
        }

        exec_post_funcs(); // execute post-loop functions serially

        tpool.stop(); // kill thread pool
        disconnect_all();
    }

    if(alog_enabled) alog.close();
    if(dlog_enabled) dat.close();
}

void Spine::exec_pre_funcs()
{ std::lock_guard<std::mutex> lock(pre_m); for(auto pre_f : pre_funcs) pre_f(); }

void Spine::exec_loop_funcs()
{ std::lock_guard<std::mutex> lock(loop_m); for(auto lf : loop_funcs) lf(); }

void Spine::exec_single_funcs()
{ 
    std::lock_guard<std::mutex> lock(single_m);
    while(!single_funcs.empty()) // run single execution functions then remove them
    {
        single_funcs.front()();
        single_funcs.erase(single_funcs.begin());
    }
}

void Spine::exec_post_funcs()
{ std::lock_guard<std::mutex> lock(post_m); for(auto post_f : post_funcs) post_f(); }

int Spine::connect_and_probe()
{
    int final_stat = 0, temp_stat;

    devs_m.lock();
    for(int i = 0; i < devices.size(); i++)
    {
        temp_stat = devices[i]->connect();
        if(temp_stat < 0)
        {
            final_stat = i;
            alog.write(devices[i]->get_name() + " connection failed");
        }
        else
        {
            temp_stat = devices[i]->probe(get_probe_cmd(i),get_probe_resp(i));
            if(temp_stat < 0)
            {
                final_stat = i;
                alog.write(devices[i]->get_name() + " probe failed");
            }
        }
    }
    devs_m.unlock();

    if(final_stat != 0) // if one or more devices failed the initialization routine
    {
        final_stat *= -1; // change sign to negative to clearly indicate failure
        disconnect_all(); // disconnect any connected devices to allow for fresh start
    }
    else
    {
        alog.write("device initialization successful");
    }

    return final_stat;
}

void Spine::query_devices(ctpl::thread_pool& tpool)
{
    int stat;
    clk::time_point current_time;

    devs_m.lock();
    for(int i = 0; i < devices.size(); i++)
    {
        current_time = clk::now();

        if(devices[i]->is_connected())
        {
            query_elapsed = current_time - get_last_query(i);
            elapsed_secs = query_elapsed.count()/1000.00;
            // run query function if available, enabled, and the assigned query interval has passed
            if(is_available(i) && devices[i]->is_enabled() && elapsed_secs > get_query_freq(i))
            {
                set_available(i,false); // mark device unavailable while query is in thread pool
                set_last_query(i,current_time);
                tpool.push([=](int id)
                { query_m.lock(); query_funcs[i](); query_m.unlock(); set_available(i,true); });
            }
        }
        else
        {
            query_elapsed = current_time - last_recons[i];
            elapsed_secs = query_elapsed.count()/1000.00;

            if(elapsed_secs > recon_attempt_freqs[i])
            {
                stat = devices[i]->connect();
                if(stat == 0)
                {
                    stat = devices[i]->probe(probe_cmds[i],probe_resps[i]);
                    if(stat < 0)
                    {
                        alog.write(devices[i]->get_name() + " reconnected successfully, but probe failed");
                        devices[i]->disconnect(); // disconnect and try again
                    }
                    else
                    {
                        alog.write(devices[i]->get_name() + " reconnected and probed successfully");
                    }
                }
                last_recons[i] = current_time;
            }
        }
    }
    devs_m.unlock();
}

double Spine::get_query_freq(int index)
{ std::lock_guard<std::mutex> lock(query_freq_m); return query_freqs[index]; }

clk::time_point Spine::get_last_query(int index)
{ std::lock_guard<std::mutex> lock(last_query_m); return last_queries[index]; }

void Spine::set_last_query(int index, clk::time_point time)
{ std::lock_guard<std::mutex> lock(last_query_m); last_queries[index] = time; }

std::string Spine::get_probe_cmd(int index)
{ std::lock_guard<std::mutex> lock(probe_cmd_m); return probe_cmds[index]; }

std::string Spine::get_probe_resp(int index)
{ std::lock_guard<std::mutex> lock(probe_resp_m); return probe_resps[index]; }

void Spine::disconnect_all()
{
    std::lock_guard<std::mutex> lock(devs_m);
    for(auto dev : devices)
    { if(dev->is_connected()) dev->disconnect(); }
}

bool Spine::is_available(int index)
{ std::lock_guard<std::mutex> lock(available_m); return devs_available[index]; }

void Spine::set_available(int index, bool state)
{ std::lock_guard<std::mutex> lock(available_m); devs_available[index] = state; }

bool err(std::string str)
{ return contains(str,"ERR"); }

void remove_all(std::string &str, std::string sub)
{ str.erase(std::remove(str.begin(), str.end(), sub), str.end()); }

double to_doub(std::string str)
{
    double num = std::strtod(str.c_str(),nullptr);
    if(num == 0.0 && str[0] != '0')
        return -1;
    return num;
}

std::string to_str(auto &num)
{ return std::to_string(num); }

int Spine::parse_config()
{ 
    struct stat buffer;   
    bool file_exists = (stat("config", &buffer) == 0);
    if(!file_exists)
        return -1; // exit function w/ error if file not found

    std::ifstream config_file("config");
    std::string line, value;
    std::string title_str, value_str;
    size_t bracket;

    config_map.clear(); // clear map in case this is a re-run

    while (getline(file, line))
    {
        bracket = line.find("[");
        if(bracket != std::string::npos)
        {
            title_str = line.substr(0,bracket);
            value_str = line.substr(bracket+1,(line.find("]")-bracket-1));
            config_map.insert({title_str,value_str});
        }
    }

    config_file.close();
    return 0;
}

std::string Spine::config_get_str(std::string target, bool &ok)
{
    for (std::pair<std::string,std::string> item : config_map)
    {
        if(contains(item.first,target))
            return item.second;
    }

    ok = false;
    return "ERROR: not found";
}

double Spine::config_get_double(std::string target, bool &ok)
{
    double val; // using streams in order to support scientific notation
    std::stringstream ss(config_get_str(target,ok));
    ss >> val;
    return val;
}

bool Spine::config_get_bool(std::string target, bool &ok)
{
    std::string value = config_get_str(target,ok);
    std::transform(value.begin(), value.end(), value.begin(), ::tolower);
    return contains(value,"true");
}

bool Spine::config_entry_empty(std::string target, bool &ok)
{ return (config_get_str(target,ok) == ""); }

std::string Spine::shout(std::string str)
{
    std::lock_guard<std::mutex> lock(cout_m);
    std::cout << str << "\n";
    return str;
}

bool approx_equal(double a, double b)
{ return (a >= b - 0.000001 && a <= b + 0.000001); }

bool isOK(std::string str)
{ return contains(str,"OK"); }

bool contains(std::string str, std::string sub)
{ return (str.find(sub) != std::string::npos); }

bool starts_with(std::string str, std::string sub)
{ return (str.find(sub) == 0); }

std::string dec2hex(int num)
{
    std::stringstream ss;
    ss << std::hex << num;
    std::string result = ss.str();
    transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

int hex2dec(std::string str)
{
    int result;
    std::stringstream ss(str);
    ss >> std::hex >> result;
    return result;
}

std::string hex2bin(std::string str)
{
    unsigned long i = 0;
    std::string result = "";

    while (str[i])
    {
        switch (str[i])
        {
        case '0':
            result += "0000";
            break;
        case '1':
            result += "0001";
            break;
        case '2':
            result += "0010";
            break;
        case '3':
            result += "0011";
            break;
        case '4':
            result += "0100";
            break;
        case '5':
            result += "0101";
            break;
        case '6':
            result += "0110";
            break;
        case '7':
            result += "0111";
            break;
        case '8':
            result += "1000";
            break;
        case '9':
            result += "1001";
            break;
        case 'A':
        case 'a':
            result += "1010";
            break;
        case 'B':
        case 'b':
            result += "1011";
            break;
        case 'C':
        case 'c':
            result += "1100";
            break;
        case 'D':
        case 'd':
            result += "1101";
            break;
        case 'E':
        case 'e':
            result += "1110";
            break;
        case 'F':
        case 'f':
            result += "1111";
            break;
        default:
            return "ERROR";
        }
        i++;
    }
    return result;
}

std::string sci_str(double val, int prec)
{
    std::stringstream converter;
    converter << std::setprecision(prec) << std::scientific << val;
    return converter.str();
}

std::vector<std::string> Spine::get_sys_errors()
{ std::lock_guard<std::mutex> lock(sys_error_m); return sys_errors; }

std::vector<std::string> Spine::get_sys_warnings()
{ std::lock_guard<std::mutex> lock(sys_warning_m); return sys_warnings; }

void Spine::log_event(std::string str) { elog.write(str); }
void Spine::log_data(std::string str) { dlog.write(str); }

std::string Spine::get_event_history() { elog.get_history(); }
std::string Spine::get_data_history() { dlog.get_history(); }
