#ifndef GYROTRON_H
#define GYROTRON_H

#include <atomic>
#include <algorithm>
#include <unistd.h>
#include <math.h>
#include <iomanip> // used to set precision of a number in string stream
#include "lib/spine/spine.h"
#include "lib/pid.h"

class Gyrotron : public Spine
{
public:
    Gyrotron();
    int init();
    int increment_state();
    int decrement_state();
    int enter_standby(); // stop PID, turn off HV, return to state 0 right away (dangerous)
    int extract_config(); // extract configuration parameters from config file
    int prepare_all(); // apply all setpoints necessary for operation

    void set_ramp_time(double time);
    void set_ramp_sp(double sp);
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

    int toggle_mw(bool turn_on); // return -1 if either cathde or GTC fail to turn on
    int toggle_cath_output(bool turn_on);
    int toggle_gtc_output(bool turn_on);
    void toggle_beam_pid();
    void toggle_beam_pid(bool turn_on);
    void toggle_power_pid();
    void toggle_power_pid(bool turn_on);
    void toggle_freq_pid();
    void toggle_freq_pid(bool turn_on);

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
    bool beam_pid_is_on() { return beam_pid_on; }
    bool power_pid_is_on() { return power_pid_on; }
    bool freq_pid_is_on() { return freq_pid_on; }

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
    int get_state() { return current_state; }
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
    std::vector<double> get_press_data() { std::lock_guard<std::mutex> lock(press_m); return press_data; }
    std::vector<double> get_press_time_data() { std::lock_guard<std::mutex> lock(press_m); return press_time_data; }
    std::vector<double> get_beam_data() { std::lock_guard<std::mutex> lock(beam_m); return beam_data; }
    std::vector<double> get_beam_time_data() { std::lock_guard<std::mutex> lock(beam_m); return beam_time_data; }
    std::vector<double> get_power_data() { std::lock_guard<std::mutex> lock(power_m); return power_data; }
    std::vector<double> get_power_time_data() { std::lock_guard<std::mutex> lock(power_m); return power_time_data; }
    std::vector<std::string> get_warnings();
    std::vector<std::string> get_errors();

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

    // ************* MANY CONSTANTS STILL TBD ********************

    // fault thresholds
    const double WARN_BC_DIFF{-1}, FATAL_BC_DIFF{-1}; // allowed diff between body and collector currents
    const double WARN_TEMP{-1}, FATAL_TEMP{-1};
    const double WARN_WATER_FLOW{-1}, FATAL_WATER_FLOW{-1};
    const double WARN_AIR_FLOW{-1}, FATAL_AIR_FLOW{-1};

    // hardware limits
    const double MAX_BEAM_VOLT{20}, MAX_BEAM_CURR{30}, MAX_FIL_CURR{5};
    const double MAX_GTC_VOLT{-1}, MAX_GTC_CURR{-1};

    // operational limits (also used for OVP/OCP values)
    const double BEAM_VOLT_LIMIT{-1}, BEAM_CURR_LIMIT{-1}, FIL_CURR_LIMIT{3.3};
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

    Device cath{"DXM","Cathode"}, gtc{"ACOPIAN","GTC"}, rsi{"RSI"}, spc{"GAMMA_VAC","SPC"}, fms{"FMS"};

    PID *beam_pid, *power_pid, *freq_pid;
    double pid_dt;
    clk::time_point last_pid_time;

    std::atomic<double> beam_kp{0}, beam_ki{0}, beam_kd{0};
    std::atomic<double> power_kp{0}, power_ki{0}, power_kd{0};
    std::atomic<double> freq_kp{0}, freq_ki{0}, freq_kd{0};
    std::atomic<double> e_ramp_rate{0.5}, e_ramp_time, ramp_rate{0}, ramp_sp{0}, ramp_time{0};
    double press_bound1{8e-8}, press_bound2{4e-7}, press_bound3{1e-6}, fatal_press{5e-6};
    double power_limit, power_calibrate;

    std::chrono::duration<double,std::milli> pid_elapsed;
    clk::time_point last_recording{clk::now()};

    std::atomic<int> current_state{0};
    std::atomic<double> body_curr{0}, collector_curr{0}, beam_ocp;
    std::atomic<double> pressure{0}, query_rate{0.5}, rec_rate{0};
    std::atomic<double> freq, power, plot_span;
    std::atomic<double> beam_curr_sp{-1}, power_sp{-1}, freq_sp{-1}, beam_volt_sp{0};
    std::atomic<double> fil_curr, fil_curr_sp{0}, beam_curr{0}, beam_volt{0}, beam_volt_ref{0};
    std::atomic<double> gtc_curr{0}, gtc_volt{0};
    std::atomic<double> diode_volt{-1}, body_volt{-1}, collector_volt{-1};
    std::atomic<double> main_chill_temp{-1}, cav_chill_temp{-1}, collector_temp{-1}, cav_temp{-1}, body_temp{-1};
    std::atomic<double> main_chill_flow{-1}, cav_chill_flow{-1}, collector_flow{-1}, gun_air_flow{-1};
    std::atomic<double> gtc_curr_sp{0}, gtc_volt_sp{0};

    std::atomic<bool> gtc_hv_on{false}, cath_hv_on{false}, cath_hv_on_prev{false};
    std::atomic<bool> e_ramping{false}, hv_blocked{false}, reset_pid_time{false};
    std::atomic<bool> freq_pid_on{false}, beam_pid_on{false}, power_pid_on{false};
    std::atomic<bool> freq_pid_on_prev{false}, beam_pid_on_prev{false}, power_pid_on_prev{false};
    std::atomic<bool> ramping_up{false}, ramping_down{false};
    std::atomic<bool> relaxing{false}, paused{false};

    std::mutex press_m, beam_m, power_m;
    std::vector<double> press_data, press_time_data, beam_data, beam_sp_data, beam_time_data;
    std::vector<double> power_array, power_sp_data, power_data, power_time_data;
};

#endif
