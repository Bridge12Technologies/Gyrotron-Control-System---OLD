#ifndef GYROTRON_H
#define GYROTRON_H

#include <math.h>
#include <limits> // for NaN
#include "lib/spine/spine.h"
#include "lib/pid.h"

class Gyrotron : public Spine
{
public:
    bool gui_debug_mode{true}; // SET TRUE TO DISABLE BACKEND

    Gyrotron();
    int init();
    int increment_state();
    int decrement_state();
    int enter_standby(); // stop PID, turn off HV, return to state 0 right away (dangerous)
    int extract_config(); // extract configuration parameters from config file
    int prepare_all(); // apply all setpoints necessary for operation

    void set_rec_rate(double secs) { rec_rate = secs; }
    void set_ramp_time(double secs) { ramp_time = secs; }
    void set_ramp_sp(double sp) { ramp_sp = sp; }
    int set_fil_curr_limit(double current);
    int set_beam_curr_limit(double current);
    int set_gtc_curr_limit(double current);
    int set_gtc_volt(double voltage);
    int set_gtc_curr(double current);
    int set_fil_curr(double current);
    int set_beam_volt(double voltage);
    void set_beam_curr(double current);
    void set_freq(double freq); // set setpoint for frequency PID
    void set_power(double power); // set setpoint for power PID
    void set_beam_kp(double kp);
    void set_beam_ki(double ki);
    void set_beam_kd(double kd);
    void set_power_kp(double kp);
    void set_power_ki(double ki);
    void set_power_kd(double kd);
    void set_freq_kp(double kp);
    void set_freq_ki(double ki);
    void set_freq_kd(double kd);
    void set_plot_span(double secs) { plot_span = secs; }

    int toggle_mw(bool turn_on); // return -1 if either cathde or GTC fail to turn on
    int toggle_cath_output(bool turn_on);
    int toggle_gtc_output(bool turn_on);
    void toggle_beam_pid();
    void toggle_beam_pid(bool turn_on);
    void toggle_power_pid();
    void toggle_power_pid(bool turn_on);
    void toggle_freq_pid();
    void toggle_freq_pid(bool turn_on);
    void turn_off_pid();

    bool all_clear(); // true if all devs enabled, connected, & no errors
    bool is_paused() { return paused; }
    bool is_ramping_down() { return ramping_down; }
    bool is_ramping_up() { return ramping_up; }
    bool is_e_ramping() { return e_ramping; }
    bool cath_is_enabled() { return cath.is_enabled(); }
    bool gtc_is_enabled() { return gtc.is_enabled(); }
    bool spc_is_enabled() { return spc.is_enabled(); }
    bool rsi_is_enabled() { return rsi.is_enabled(); }
    bool fms_is_enabled() { return fms.is_enabled(); }
    bool cath_is_connected() { return cath.is_connected(); }
    bool gtc_is_connected() { return gtc.is_connected(); }
    bool spc_is_connected() { return spc.is_connected(); }
    bool rsi_is_connected() { return rsi.is_connected(); }
    bool fms_is_connected() { return fms.is_connected(); }
    bool cath_available() { return (cath.is_enabled() && cath.is_connected()); }
    bool gtc_available() { return (gtc.is_enabled() && gtc.is_connected()); }
    bool spc_available() { return (spc.is_enabled() && spc.is_connected()); }
    bool rsi_available() { return (rsi.is_enabled() && rsi.is_connected()); }
    bool fms_available() { return (fms.is_enabled() && fms.is_connected()); }

    bool beam_pid_is_on() { return beam_pid_on; }
    bool power_pid_is_on() { return power_pid_on; }
    bool freq_pid_is_on() { return freq_pid_on; }
    bool pid_is_on() { return (beam_pid_on || power_pid_on || freq_pid_on); }

    int get_fault_status(); // 0 = no faults, -1 = warnings, -2 = errors
    int get_temp_status();  // 0 = safe temp, -1 = warning, -2 = fatal
    int get_flow_status();  // 0 = safe flow, -1 = warning, -2 = fatal
    int get_press_status(); // 0 = safe pressure, -1 = relax, -2 = fatal

    double get_fil_curr_sp() { return fil_curr_sp; }
    double get_beam_volt_sp() { return beam_volt_sp; }
    double get_beam_curr_sp() { return beam_curr_sp; }
    double get_beam_ocp() { return beam_ocp; }
    double get_freq_sp() { return freq_sp; }
    double get_power_sp() { return power_sp; }
    double get_gtc_curr_sp() { return beam_curr_sp; }
    double get_gtc_volt_sp() { return beam_curr_sp; }
    int get_state() { return current_state; } // 0 = ctrl power on, 1 = ramping, 2 = HV standby, 3 = MW on
    double get_fil_curr() { return fil_curr; }
    double get_beam_volt() { return beam_volt; }
    double get_beam_curr() { return beam_curr; }
    double get_collector_curr() { return collector_curr; }
    double get_body_curr() { return body_curr; }
    double get_freq() { return freq; }
    double get_power() { return power; }
    double get_temp() { return body_temp; }
    double get_pressure() { return pressure; }
    double get_gtc_curr() { return gtc_curr; }
    double get_gtc_volt() { return gtc_volt; }
    void get_pid_consts(double& p, double& i, double& d);
    void get_fpid_consts(double& p, double& i, double& d);
    double get_ramp_time() { return ramp_time; }
    double get_ramp_sp() { return ramp_sp; }
    bool cath_output_is_on() { return  }

    std::vector<double> get_press_data() { std::lock_guard<std::mutex> lock(press_m); return press_data; }
    std::vector<double> get_press_time_data() { std::lock_guard<std::mutex> lock(press_m); return press_time_data; }
    void clear_press_data() { std::lock_guard<std::mutex> lock(press_m); press_data.clear(); press_time_data.clear(); }

    std::vector<double> get_beam_curr_data() { std::lock_guard<std::mutex> lock(beam_curr_m); return beam_curr_data; }
    std::vector<double> get_beam_curr_sp_data() { std::lock_guard<std::mutex> lock(beam_curr_m); return beam_curr_sp_data; }
    std::vector<double> get_beam_curr_time_data() { std::lock_guard<std::mutex> lock(beam_curr_m); return beam_curr_time_data; }
    void clear_beam_curr_data() { std::lock_guard<std::mutex> lock(beam_curr_m); beam_curr_data.clear(); beam_curr_sp_data.clear(); beam_curr_time_data.clear(); }

    std::vector<double> get_beam_volt_data() { std::lock_guard<std::mutex> lock(beam_volt_m); return beam_volt_data; }
    std::vector<double> get_beam_volt_sp_data() { std::lock_guard<std::mutex> lock(beam_volt_m); return beam_volt_sp_data; }
    std::vector<double> get_beam_volt_time_data() { std::lock_guard<std::mutex> lock(beam_volt_m); return beam_volt_time_data; }
    void clear_beam_volt_data() { std::lock_guard<std::mutex> lock(beam_volt_m); beam_volt_data.clear(); beam_volt_sp_data.clear(); beam_volt_time_data.clear(); }

    std::vector<double> get_power_data() { std::lock_guard<std::mutex> lock(power_m); return power_data; }
    std::vector<double> get_power_sp_data() { std::lock_guard<std::mutex> lock(power_m); return power_sp_data; }
    std::vector<double> get_power_time_data() { std::lock_guard<std::mutex> lock(power_m); return power_time_data; }
    void clear_power_data() { std::lock_guard<std::mutex> lock(power_m); power_data.clear(); power_sp_data.clear(); power_time_data.clear(); }

    std::vector<double> get_fil_curr_data() { std::lock_guard<std::mutex> lock(fil_curr_m); return fil_curr_data; }
    std::vector<double> get_fil_curr_sp_data() { std::lock_guard<std::mutex> lock(fil_curr_m); return fil_curr_sp_data; }
    std::vector<double> get_fil_curr_time_data() { std::lock_guard<std::mutex> lock(fil_curr_m); return fil_curr_time_data; }
    void clear_fil_curr_data() { std::lock_guard<std::mutex> lock(fil_curr_m); fil_curr_data.clear(); fil_curr_sp_data.clear(); fil_curr_time_data.clear(); }

    std::vector<double> get_freq_data() { std::lock_guard<std::mutex> lock(freq_m); return freq_data; }
    std::vector<double> get_freq_sp_data() { std::lock_guard<std::mutex> lock(freq_m); return freq_sp_data; }
    std::vector<double> get_freq_time_data() { std::lock_guard<std::mutex> lock(freq_m); return freq_time_data; }
    void clear_freq_data() { std::lock_guard<std::mutex> lock(freq_m); freq_data.clear(); freq_sp_data.clear(); freq_time_data.clear(); }

    std::vector<std::string> get_warnings();
    std::vector<std::string> get_errors();
    unsigned int get_num_warnings() { return unsigned(get_warnings().size()); }
    unsigned int get_num_errors() { return unsigned(get_errors().size()); }
    double get_plot_span() { return plot_span; }

    double get_temp(int num); // 1=main chiller, 2=cavity chiller, 3=collector, 4=cavity, 5=body
    double get_flow(int num); // 1=main chiller, 2=cavity chiller, 3=collector, 4=gun air
    double get_main_chill_temp() { return main_chill_temp; }
    double get_cav_chill_temp() { return cav_chill_temp; }
    double get_collector_temp() { return collector_temp; }
    double get_cavity_temp() { return cav_temp; }
    double get_body_temp() { return body_temp; }
    double get_main_chill_flow() { return main_chill_flow; }
    double get_cav_chill_flow() { return cav_chill_flow; }
    double get_collector_flow() { return collector_flow; }
    double get_gun_air_flow() { return gun_air_flow; }

    std::atomic<double>* beam_kp_ptr() { return &beam_kp; }
    std::atomic<double>* beam_ki_ptr() { return &beam_ki; }
    std::atomic<double>* beam_kd_ptr() { return &beam_kd; }
    std::atomic<double>* power_kp_ptr() { return &power_kp; }
    std::atomic<double>* power_ki_ptr() { return &power_ki; }
    std::atomic<double>* power_kd_ptr() { return &power_kd; }
    std::atomic<double>* freq_kp_ptr() { return &freq_kp; }
    std::atomic<double>* freq_ki_ptr() { return &freq_ki; }
    std::atomic<double>* freq_kd_ptr() { return &freq_kd; }
    std::atomic<double>* fil_curr_ptr() { return &fil_curr; }
    std::atomic<double>* fil_curr_sp_ptr() { return &fil_curr_sp; }
    std::atomic<double>* beam_volt_ptr() { return &beam_volt; }
    std::atomic<double>* beam_volt_sp_ptr() { return &beam_volt_sp; }
    std::atomic<double>* beam_curr_ptr() { return &beam_curr; }
    std::atomic<double>* beam_curr_sp_ptr() { return &beam_curr_sp; }
    std::atomic<double>* freq_ptr() { return &freq; }
    std::atomic<double>* freq_sp_ptr() { return &freq_sp; }
    std::atomic<double>* power_ptr() { return &power; }
    std::atomic<double>* power_sp_ptr() { return &power_sp; }
    std::atomic<double>* gtc_curr_ptr() { return &gtc_curr; }
    std::atomic<double>* gtc_curr_sp_ptr() { return &gtc_curr_sp; }
    std::atomic<double>* gtc_volt_ptr() { return &gtc_volt; }
    std::atomic<double>* gtc_volt_sp_ptr() { return &gtc_volt_sp; }

    void enable_cath() { cath.set_enabled(true); }
    void enable_gtc() { gtc.set_enabled(true); }
    void enable_spc() { spc.set_enabled(true); }
    void enable_rsi() { rsi.set_enabled(true); }
    void enable_fms() { fms.set_enabled(true); }
    int connect_cath() { return cath.connect(); }
    int connect_gtc() { return gtc.connect(); }
    int connect_spc() { return spc.connect(); }
    int connect_rsi() { return rsi.connect(); }
    int connect_fms() { return fms.connect(); }
    std::string cath_io(std::string cmd) { return cath.m_io(cmd); }
    std::string gtc_io(std::string cmd) { return gtc.m_io(cmd); }
    std::string spc_io(std::string cmd) { return spc.m_io(cmd); }
    std::string rsi_io(std::string cmd) { return rsi.m_io(cmd); }
    std::string fms_io(std::string cmd) { return fms.m_io(cmd); }

    // PLACEHOLDERS - replace once binary format for logs has been implemented
    bool log_is_bin() { return dlog.is_bin_format(); }
    bool log_is_txt() { return !dlog.is_bin_format(); }
    void set_log_in_bin(bool use_bin) { dlog.set_bin_format(use_bin); }
    void set_log_in_txt(bool use_txt) { dlog.set_bin_format(!use_txt); }

    bool admin_mode_config() { return config_get_bool("Admin Mode"); }

    // ************* MANY CONSTANTS STILL TBD ********************

    // fault thresholds
    const double WARN_BC_DIFF{-1}, FATAL_BC_DIFF{-1}; // allowed diff between body and collector currents
    const double WARN_TEMP{-1}, FATAL_TEMP{-1};
    const double WARN_WATER_FLOW{-1}, FATAL_WATER_FLOW{-1};
    const double WARN_AIR_FLOW{-1}, FATAL_AIR_FLOW{-1};

    // hardware limits
    const double MAX_BEAM_VOLT{60}, MAX_BEAM_CURR{80}, MAX_FIL_CURR{5};
    const double MAX_GTC_VOLT{-1}, MAX_GTC_CURR{-1};

    // operational limits (also used for OVP/OCP values)
    const double BEAM_VOLT_LIMIT{60}, BEAM_CURR_LIMIT{80}, FIL_CURR_LIMIT{5};
    const double GTC_VOLT_LIMIT{-1}, GTC_CURR_LIMIT{-1}; // TBD, max for now
    const double POWER_LIMIT{-1}, UPPER_FREQ_LIMIT{-1}, LOWER_FREQ_LIMIT{-1};

    // other constants
    const double POWER_V_CONVERT{-1}; // constant to convert voltage to power
    const double BODY_R{-1}, COLLECTOR_R{-1}; // resistance values for body and collector, used to get I

private:
    void query_cath(); // routine query for cathode
    void query_gtc(); // routine query for gun trim coil
    void query_spc(); // routine query for gamma vac spc
    void query_fms(); // routine query for frequency monitoring system
    void query_rsi(); // routine query for remote signal interface
    void steer_cath(); // adjust cathode voltage/current for PID & ramping
    void cath_recon(); // executed upon successful reconnect of cathode
    void gtc_recon(); // executed upon successful reconnect of gtc
    void spc_recon(); // executed upon successful reconnect of spc
    void fms_recon(); // executed upon successful reconnect of fms
    void rsi_recon(); // executed upon successful reconnect of rsi
    void record_data(); // record information to data log
    int apply_beam_curr_limit(double current); // write beam current limit to cathode
    void eval_sys_stat(); // check system status, determine system fault state
    double runtime(); // return elapsed seconds since initialization
    void update_cath_plot_data();

    Device cath{"DF","Cathode"}, gtc{"ACOPIAN","GTC"}, rsi{"RSI"}, spc{"GAMMA_VAC","SPC"}, fms{"FMS"};

    PID *beam_pid, *power_pid, *freq_pid;
    double pid_dt;
    clk::time_point last_pid_time;

    std::atomic<double> beam_kp{0}, beam_ki{0}, beam_kd{0};
    std::atomic<double> power_kp{0}, power_ki{0}, power_kd{0};
    std::atomic<double> freq_kp{0}, freq_ki{0}, freq_kd{0};
    std::atomic<double> e_ramp_rate{0.5}, e_ramp_time, ramp_rate{0}, ramp_sp{0}, ramp_time{0};
    double press_bound1{8e-8}, press_bound2{4e-7}, press_bound3{1e-6}, fatal_press{5e-6};
    double power_limit, power_calibrate;
    std::string cath_set_cmd{"S000000000000255"}; // default all setpoints zero and output off
    std::chrono::duration<double,std::milli> pid_elapsed;
    clk::time_point last_recording{clk::now()};

    std::atomic<int> current_state{0};
    std::atomic<double> body_curr{0}, collector_curr{0}, beam_ocp;
    std::atomic<double> pressure{0}, query_rate{0.5}, rec_rate{0};
    std::atomic<double> freq, power;
    std::atomic<double> beam_curr_sp{-1}, power_sp{-1}, freq_sp{-1}, beam_volt_sp{-1};
    std::atomic<double> fil_curr, fil_curr_sp{-1}, beam_curr{0}, beam_volt{0}, beam_volt_ref{0};
    std::atomic<double> gtc_curr{0}, gtc_volt{0};
    std::atomic<double> diode_volt{-1}, body_volt{-1}, collector_volt{-1};
    std::atomic<double> main_chill_temp{-1}, cav_chill_temp{-1}, collector_temp{-1}, cav_temp{-1}, body_temp{-1};
    std::atomic<double> main_chill_flow{-1}, cav_chill_flow{-1}, collector_flow{-1}, gun_air_flow{-1};
    std::atomic<double> gtc_curr_sp{-1}, gtc_volt_sp{-1};

    std::atomic<bool> gtc_hv_on{false}, cath_hv_on{false}, cath_hv_on_prev{false};
    std::atomic<bool> e_ramping{false}, hv_blocked{false}, reset_pid_time{false};
    std::atomic<bool> freq_pid_on{false}, beam_pid_on{false}, power_pid_on{false};
    std::atomic<bool> freq_pid_on_prev{false}, beam_pid_on_prev{false}, power_pid_on_prev{false};
    std::atomic<bool> ramping_up{false}, ramping_down{false};
    std::atomic<bool> relaxing{false}, paused{false};

    std::mutex press_m, beam_curr_m, beam_volt_m, power_m, fil_curr_m, freq_m, cath_cmd_m;
    std::atomic<double> plot_span{900}; // default 15 mins
    std::vector<double> press_data, beam_curr_data, beam_volt_data, power_data, fil_curr_data, freq_data;
    std::vector<double> beam_curr_sp_data, beam_volt_sp_data, power_sp_data, fil_curr_sp_data, freq_sp_data;
    std::vector<double> press_time_data, beam_curr_time_data, beam_volt_time_data, power_time_data, fil_curr_time_data, freq_time_data;
};

#endif
