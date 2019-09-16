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
    int set_gtc_volt_limit(double voltage);
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
    bool is_ramping_down(); // true if ramp down in progress
    bool is_ramping_up(); // true if ramp up in progress
    bool is_e_ramping(); // true if emergency ramp in progress
    bool cath_is_enabled();
    bool gtc_is_enabled();
    bool spc_is_enabled();
    bool rsi_is_enabled();
    bool fms_is_enabled();
    bool beam_pid_is_on();
    bool power_pid_is_on();
    bool freq_pid_is_on();

    double get_fil_curr_sp();
    double get_beam_volt_sp();
    double get_beam_curr_sp();
    double get_freq_sp();
    double get_power_sp();
    double get_gtc_curr_sp();
    double get_gtc_volt_sp();
    int get_state();
    double get_fil_curr();
    double get_beam_volt();
    double get_beam_curr();
    double get_collector_curr();
    double get_body_curr();
    double get_freq();
    double get_power();
    int get_fault_status(); // 0 = no faults, -1 = warnings, -2 = errors
    int get_temp_status(); // 0 = safe temp, -1 = warning, -2 = fatal
    double get_temp();
    int get_flow_status(); // 0 = safe temp, -1 = warning, -2 = fatal
    double get_flow();
    double get_pressure();
    int get_press_status(); // 0 = safe pressure, -1 = relax, -2 = fatal
    double get_gtc_curr();
    double get_gtc_volt();
    void get_pid_consts(double& p, double& i, double& d);
    void get_fpid_consts(double& p, double& i, double& d);
    double get_ramp_time();
    double get_ramp_sp();
    std::vector<double> get_press_data();
    std::vector<double> get_press_time_data();
    std::vector<double> get_beam_data();
    std::vector<double> get_beam_time_data();
    std::vector<double> get_power_data();
    std::vector<double> get_power_time_data();

    std::vector<std::string> get_warnings();
    std::vector<std::string> get_errors();

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

    Device cath{"DXM","Cathode"}, gtc{"RIGOL","GTC"}, rsi{"RSI"}, spc{"SPC"}, fms{"FMS"};

    PID *beam_pid, *power_pid, *freq_pid;

    // output limits
    double fil_curr_limit{}, beam_curr_limit{}, beam_volt_limit{};
    double gtc_curr_limit{}, gtc_volt_limit{};

    // input limits
    double fil_curr_max{}, beam_curr_max{}, beam_volt_max{};
    double gtc_curr_max{}, gtc_volt_max{}, power_max{}, freq_max{};

    std::atomic<double> beam_kp{0}, beam_ki{0}, beam_kd{0};
    std::atomic<double> power_kp{0}, power_ki{0}, power_kd{0};
    std::atomic<double> freq_kp{0}, freq_ki{0}, freq_kd{0};
    std::atomic<double> e_ramp_rate{0.5}, e_ramp_time, ramp_rate{0}, ramp_sp{0}, ramp_time{0};
    double press_bound1{8e-8}, press_bound2{4e-7}, press_bound3{1e-6}, fatal_press{5e-6};
    double power_limit, power_calibrate, err_limit1{10}, err_limit2{30};

    // various operational limits
    const double MAX_BODY_COL_DIFF{}; // max allowed difference between body and collector currents
    const double MAX_POWER{}, MAX_FREQ{}, MIN_FREQ{};
    const double MAX_PRESS{}, MAX_TEMP{}, MAX_DIODE_VOLT{};
    const double MIN_WATER_FLOW{}, MIN_AIR_FLOW{};

    // slave device limits
    const double MAX_BEAM_VOLT{20}, MAX_BEAM_CURR{30}, MAX_FIL_SET{3.3}, MAX_FIL_CURR{5};
    const double MAX_GTC_VOLT{}, MAX_GTC_CURR{}; // TBD

    // operational constants
    const double OP_BEAM_VOLT{}, OP_GTC_VOLT{}, OP_GTC_CURR{}; // TBD

    std::chrono::duration<double, std::milli> pid_elapsed;
    clk::time_point last_recording{clk::now()};

    std::atomic<int> current_state{0};
    std::atomic<double> pressure{0}, query_rate{0.5}, rec_rate{0};
    std::atomic<double> freq, power, plot_span;
    std::atomic<double> beam_curr_sp{-1}, power_sp{-1}, freq_sp{-1}, beam_volt_sp{0};
    std::atomic<double> fil_curr, fil_curr_sp{0}, beam_curr, beam_volt;
    std::atomic<double> gtc_curr{0}, gtc_volt{0};
    std::atomic<double> gtc_curr_sp{0}, gtc_volt_sp{0};
    std::atomic<bool> gtc_hv_on{false};
    std::atomic<bool> e_ramping{false}, hv_blocked{false}, reset_pid_time{false};
    std::atomic<bool> freq_pid_on{false}, beam_pid_on{false}, power_pid_on{false};

    bool  ramping_up{false}, ramping_down{false}, cath_fault_triggered{false};
    bool relaxing{false}, paused{false};
    std::vector<double> press_data, time_data, beam_data, beam_sp_data, beam_time_data;
    std::vector<double> power_array, power_sp_data, power_data, power_time_data;
};

#endif
