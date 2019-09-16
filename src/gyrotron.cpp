#include "gyrotron.h"

Gyrotron::Gyrotron() : Spine()
{
    elog.set_history_enabled(true);
    dlog.set_header("system time (H:M:S), pressure (Torr), etc etc");

    //add_device(&cath,[=](){query_cath();},[=](){cath_recon();},"99,1,","$");
    //add_device(&gtc,[=](){query_gtc();},[=](){gtc_recon();},"*IDN?","DP821A");
    //add_device(&rsi,[=](){query_rsi();},[=](){rsi_recon();},"hello rsi","rsi");
    //add_device(&spc,[=](){query_spc();},[=](){spc_recon();},"spc 01","00");
    //add_device(&fms,[=](){query_fms();},[=](){fms_recon();},"#0","0");

    add_pre_func([=](){apply_beam_curr_limit(beam_curr_limit);});
    add_pre_func([=](){gtc.m_write(":SYST:REM");});

    add_loop_func([=](){record_data();});
    add_loop_func([=](){eval_sys_stat();});
}

int Gyrotron::init()
{
    if(extract_config() < 0)
        return -1;
    return Spine::init();
}

int Gyrotron::extract_config()
{
    bool ok = true;
    int i;

    std::string titles[21] = {"Current PID Proportional","Current PID Integral","Current PID Derivative",
                            "Power PID Proportional","Power PID Integral","Power PID Derivative",
                            "Frequency PID Proportional","Frequency PID Integral","Frequency PID Derivative",
                            "Filament Ramp Set Point","Normal Ramp Time","Emergency Ramp Time","Fatal Pressure Limit",
                            "Filament Pressure High Limit","Filament Pressure Low Limit","HV Pressure Low Limit",
                            "Beam Current Limit","Output Power Limit","Power Meter Calibration",
                            "Error Count First Limit","Error Count Second Limit"};
    std::atomic<double> *a_values[12] = {&beam_kp,&beam_ki,&beam_kd,&power_kp,&power_ki,&power_kd,&freq_kp,&freq_ki,&freq_kd,
                                       &ramp_sp,&ramp_time,&e_ramp_time};
    double *values[9] = {&fatal_press,&press_bound3,&press_bound2,&press_bound1,&beam_curr_limit,
                          &power_limit,&power_calibrate,&err_limit1,&err_limit2};

    for(i = 0; i < 12; i++) { *a_values[i] = config_get_double(titles[i],ok); }
    for(i = 0; i < 9; i++) { *values[i] = config_get_double(titles[i+12],ok); }

    cath.set_enabled(config_get_bool("Cathode Enabled",ok));
    gtc.set_enabled(config_get_bool("GTC Enabled",ok));
    rsi.set_enabled(config_get_bool("RSI Enabled",ok));
    spc.set_enabled(config_get_bool("SPC Enabled",ok));
    fms.set_enabled(config_get_bool("FMS Enabled",ok));

    if(!ok) return -1;

    ramp_rate = (ramp_sp/(ramp_time*60))*query_rate; // calculate ramp rate from time and sp
    e_ramp_rate = (3/e_ramp_time)*query_rate;

    beam_pid = new PID(beam_kp, beam_ki, beam_kd);
    power_pid = new PID(power_kp, power_ki, power_kd);
    freq_pid = new PID(freq_kp, freq_ki, freq_kd);

    return 0;
}

void Gyrotron::query_cath()
{
    std::stringstream ss;
    double temp_double, temp_int;
    char comma;
    // get filament current and beam current monitors
    std::string resp = cath.smart_io("19,","19,",true);
    if(!err(resp))
    {
        resp.erase(0,3); // erase first 3 chars with echoed cmd
        ss.str(resp);
        ss >> temp_double; ss >> comma;
        beam_volt = (temp_double/4095)*MAX_BEAM_VOLT;
        ss >> temp_double; ss >> comma;
        beam_curr = (temp_double/4095)*MAX_BEAM_CURR;
        ss >> temp_double;
        fil_curr = (temp_double/4095)*MAX_FIL_CURR;

        cath.error_m()->lock();
        cath.warning_m()->lock();
        cath.clear_errors();
        cath.clear_warnings();

        resp = cath.smart_io("68,","68,",true);
        if(!err(resp) && resp.length() > 10)
        {
            resp.erase(0,3);
            ss.clear();
            ss.str(resp);
            ss >> temp_int; ss >> comma;
            if(int(temp_int) == 1) cath.push_error("Cathode: arc detected");
            ss >> temp_int; ss >> comma;
            if(int(temp_int) == 1) cath.push_error("Cathode: over temperature");
            ss >> temp_int; ss >> comma;
            if(int(temp_int) == 1) cath.push_warning("Cathode: over voltage");
            ss >> temp_int; ss >> comma;
            if(int(temp_int) == 1) cath.push_warning("Cathode: under voltage");
            ss >> temp_int; ss >> comma;
            if(int(temp_int) == 1) cath.push_warning("Cathode: over current");
            ss >> temp_int;
            if(int(temp_int) == 1) cath.push_warning("Cathode: under current");

        }
        else if(err(resp))
            elog.write("Cathode D/C, moving to reconnect loop");
        
        cath.warning_m()->unlock(); 
        cath.error_m()->unlock();

        // add in plotting vector management here
        beam_time_data.push_back(runtime());
        beam_data.push_back(beam_curr);
        beam_sp_data.push_back(beam_curr_sp);

        while(runtime() - *beam_time_data.begin() > plot_span)
        {
            if(!beam_data.empty()) beam_data.erase(beam_data.begin());
            if(!beam_sp_data.empty()) beam_sp_data.erase(beam_sp_data.begin());
            if(!beam_time_data.empty()) beam_time_data.erase(beam_time_data.begin());
        }

        steer_cath();
    }
    else  
        elog.write("Cathode D/C, moving to reconnect loop");
}

void Gyrotron::query_gtc()
{
    std::stringstream ss;
    double temp_double;

    std::string resp = gtc.smart_io(":MEAS:VOLT:DC? CH1","error",false);
    if(!err(resp))
    {
        temp_double = to_doub(resp);
        if(temp_double >= 0) // check conversion
            gtc_volt = temp_double;
        else
            elog.write(shout("invalid gtc voltage conversion: " + resp));
        
        resp = gtc.smart_io(":MEAS:CURR:DC? CH1","error",false);
        if(!err(resp))
        {
            temp_double = to_doub(resp);
            if(temp_double >= 0) // check conversion
                gtc_curr = temp_double;
            else
                elog.write(shout("invalid gtc current conversion: " + resp));

            resp = gtc.smart_io(":OUTP? CH1");
            if(!err(resp))
            {
                if(contains(resp,"ON"))
                    gtc_hv_on = true;
                else if(contains(resp,"OFF"))
                    gtc_hv_on = false;
                else
                    elog.write("couldn't read GTC HV status: " + resp);
                
                resp = gtc.smart_io(":SYST:ERR?");
                if(!err(resp))
                {
                    gtc.error_m()->lock();
                    gtc.warning_m()->lock();
                    gtc.clear_errors();
                    gtc.clear_warnings();

                    if(contains(resp,"-"))
                        gtc.push_warning("GTC: " + resp);

                    gtc.warning_m()->unlock();
                    gtc.error_m()->unlock();
                }
                else
                    elog.write("GTC D/C, moving to reconnect loop");
            }
            else
                elog.write("GTC D/C, moving to reconnect loop");
        }
        else
            elog.write("GTC D/C, moving to reconnect loop");
    }
    else
        elog.write("GTC D/C, moving to reconnect loop");
}

void Gyrotron::query_spc()
{/*
    double temp_double;
    std::string resp = spc.smart_io("spc 0B","Torr",true);
    if(!err(resp))
    {
        // erase everything after the space before "Torr"
        resp.erase(resp.find("T")-1, (resp.length()-resp.find("T")-1));
        remove_all(resp,"\x20");
        temp_double = to_doub(resp);
        if(temp_double >= 0)
            pressure = temp_double;
        else
            elog.write(shout("invalid pressure conversion: " + resp));

        if(pressure >= fatal_press && !e_ramping)
        {
            e_ramping = true;
            elog.write("hit fatal pressure limit, begin relaxation state");
        }
        if(pressure >= press_bound3 && !relaxing)
        {
            relaxing = true;
            elog.write("hit pressure high limit for filament, begin relaxation state");
        }
        else if(pressure <= press_bound2 && relaxing)
        {
            relaxing = false;
            elog.write("hit pressure low limit for filament, end relaxation state");
        }

        if(pressure >= press_bound2 && !dxm_hv_blocked && dxm_hv_on)
        {
            set_cath_output_enabled(false);
            dxm_hv_on_prev = dxm_hv_on;
            dxm_hv_on = false;
            dxm_hv_blocked = true;
            
            freq_pid_on_prev = freq_pid_on;
            freq_pid_on = false;
            power_pid_on_prev = power_pid_on;
            power_pid_on = false;
            beam_curr_pid_on_prev = beam_curr_pid_on;
            beam_curr_pid_on = false;
            
            elog.write("hit pressure high limit for HV, begin blocking HV");
        }
        else if(pressure <= press_bound1 && dxm_hv_blocked)
        {
            dxm_hv_blocked = false;
            if(dxm_hv_on_prev) { if(set_cath_output_enabled(true) >= 0) { dxm_hv_on = true; }}

            freq_pid_on = freq_pid_on_prev;
            power_pid_on = power_pid_on_prev;
            beam_curr_pid_on = beam_curr_pid_on_prev;

            elog.write("hit pressure low limit for HV, stop blocking HV");
        }

        // add in plotting vector management here
        press_time_data.push_back(runtime());
        press_data.push_back(pressure);

        while(runtime() - *press_time_data.begin() > plot_span)
        {
            if(!press_data.empty()) press_data.erase(press_data.begin());
            if(!press_time_data.empty()) press_time_data.erase(press_time_data.begin());
        }
    }
    else
        elog.write("SPC D/C, moving to reconnect loop");
*/}

void Gyrotron::query_fms()
{
    double temp_double;

    std::string resp = fms.smart_io("#F","#F OK",true);
    if(!err(resp))
    {
        resp.erase(0,6); // erase "#2 OK "
        temp_double = to_doub(resp);
        if(temp_double > 0)
            freq = temp_double;
        else
            elog.write("FMS bad conversion: " + resp);
    }
    else
        elog.write("FMS D/C, moving to reconnect loop");
}

void Gyrotron::query_rsi()
{/*
    std::string cmds[] = {"mpio 1 ar 1","temp 1","flow 1"};
    std::atomic<double> *values[] = {&diode_volt, &body_temp, &flow};
    std::string resp;
    int stat = 0;

    for(int i = 0; i < 3; i++)
    {
        if(stat == 0)
        {
            resp = rsi.smart_io(cmds[i]);
            if(!err(resp)) *values[i].store(to_doub(resp));
            else stat = -1;
        }
        if(stat < 0) break;
    }

    if(diode_volt >= 0)
        power = diode_volt * POWER_V_CONVERT;

    // add in plotting vector management here
    power_time_data.push_back(runtime());
    power_data.push_back(power);
    power_sp_data.push_back(power_sp);

    while(runtime() - *power_time_data.begin() > plot_span)
    {
        if(!power_data.empty()) power_data.erase(power_data.begin());
        if(!power_sp_data.empty()) power_sp_data.erase(power_sp_data.begin());
        if(!power_time_data.empty()) power_time_data.erase(power_time_data.begin());
    }

    if(stat < 0)
        elog.write("SPC D/C, moving to reconnect loop");
*/}

void Gyrotron::eval_sys_stat()
{
    Device *devs[] = {&cath,&gtc,&spc,&rsi,&fms};

    sys_warning_m.lock();
    sys_warnings.clear();
    if(relaxing && !e_ramping)
        sys_warnings.push_back("hit high pressure limit, relaxation in progress");
    sys_warning_m.unlock();

    sys_error_m.lock();
    sys_errors.clear();
    if(e_ramping)
        sys_errors.push_back("hit fatal pressure limit, emergency ramp down in progress");

    for(Device *dev : devs) {
        if(dev->is_enabled() && !(dev->is_connected()))
            sys_errors.push_back(dev->get_name() + " is disconnected! Reconnection attempts in progresss...");
    }
    sys_error_m.unlock();
}

std::vector<std::string> Gyrotron::get_warnings()
{
    std::vector<std::string> warnings, temp;

    // collect cathode warnings
    warnings = cath.m_get_warnings();
    // collect GTC warnings
    temp = gtc.m_get_warnings();
    warnings.insert(warnings.end(), temp.begin(), temp.end());
    // collect system warnings
    sys_warning_m.lock();
    temp = sys_warnings;
    sys_warning_m.unlock();
    warnings.insert(warnings.end(), temp.begin(), temp.end());

    return warnings;
}

std::vector<std::string> Gyrotron::get_errors()
{
    std::vector<std::string> errors, temp;

    // collect cathode errors 
    errors = cath.m_get_errors();
    // collect system errors
    sys_error_m.lock();
    temp = sys_errors;
    sys_error_m.unlock();
    errors.insert(errors.end(), temp.begin(), temp.end());

    return errors;
}

void Gyrotron::steer_cath()
{/*
    int stat;
    clk::time_point current_time = clk::now();
    
    bool adjust_needed = (ramping_up || ramping_down || e_ramping || // ramp in progress
                          (beam_curr_pid_on && beam_curr_sp >= 0) || // beam current PID active
                          (power_pid_on && power_sp >= 0) || // power PID active
                          (freq_pid_on && freq_sp >= 0)); // frequency PID active

    if(cath.is_connected() && adjust_needed)
    {
        if(e_ramping)
        {
            fil_curr_sp -= e_ramp_rate;
            if(fil_curr_sp <= 0) // check if finished
            {
                e_ramping = false;
                current_state = 0;
                elog.write("emergency ramp down finished");
            }
        }
        else if(ramping_up && !relaxing)
        {
            fil_curr_sp += ramp_rate; // increment filament current
            if(fil_curr_sp >= ramp_sp) // check if finished
            {
                ramping_up = false;
                current_state = 2;
                elog.write("filament ramp up finished");
            }
        }
        else if (ramping_down && !relaxing)
        {
            fil_curr_sp -= ramp_rate; // decrement filament current
            if(fil_curr_sp <= 0) // check if finished
            {
                ramping_down = false;
                current_state = 0;
                elog.write("filament ramp down finished");
            }
        }
        else if(beam_curr_pid_on && beam_curr_sp >= 0 && !relaxing && !cath_hv_blocked) // beam current PID control
        {
            if(reset_pid_time) // set default dt if pid control is just starting/resuming
            {
                pid_dt = query_rate;
                fil_curr_sp = fil_curr;
                reset_pid_time = false;
            }
            else // otherwise calculate the dt precisely
            {
                pid_elapsed = current_time - last_pid_time;
                pid_dt = pid_elapsed.count()/1000.0;
            }

            if(pid_dt <= 0) pid_dt = query_rate;

            // calculate neccisary adjustment from PID calculation
            beam_curr_adjust = pid->calculate(beam_curr_sp,beam_curr,pid_dt);
            //fil_curr_sp += (double)(beam_curr_adjust * beam2fil_scale); // scale the adjustment to beam current for filament

            // be careful that this adjustment isn't problematic, especially prior to proper tuning
            if((beam_curr+beam_curr_adjust) > 30) { beam_curr_adjust = 30 - beam_curr; }
            else if((beam_curr+beam_curr_adjust) < 0) { beam_curr_adjust = beam_curr * (-1); }

            fil_curr_sp += beam_curr_adjust;

            last_pid_time = current_time;
        }
        else if(power_pid_on && power_sp >= 0 && power_array.size() >= 3 && !relaxing && !cath_hv_blocked) // power PID control
        {
            if(reset_pid_time) // set default dt if pid control is just starting/resuming
            {
                pid_dt = query_rate;
                fil_curr_sp = fil_curr;
                reset_pid_time = false;
            }
            else // otherwise calculate the dt precisely
            {
                pid_elapsed = current_time - last_pid_time;
                pid_dt = pid_elapsed.count()/1000.0;
            }

            if(pid_dt <= 0) pid_dt = query_rate;

            // calculate neccisary adjustment from PID calculation
            //power_adjust = pid->calculate(power_sp,power,dt);
            power_adjust = pid->calculate(power_sp,power_avg,dt);

            // be careful that this adjustment isn't problematic, especially prior to proper tuning
            //if((power+power_adjust) > 5)
            //{
            //    power_adjust = 5 - power;
            //}
            //else if((power+power_adjust) < 0)
            //{
            //    power_adjust = power * (-1);
            //}

            //fil_curr_sp += power_adjust;
            fil_curr_sp += power_adjust * 15; // trying out a scale to keep roughly the same PID constants for both controllers

            last_pid_time = current_time;
        }
        else if (freq_pid_on && freq_sp > 0 && !relaxing && !dxm_hv_blocked)
        {
            if(reset_pid_time) // set default dt if pid control is just starting/resuming
            {
                pid_dt = query_rate;
                fil_curr_sp = fil_curr;
                beam_volt_sp = beam_volt;
                reset_pid_time = false;
            }
            else // otherwise calculate the dt precisely
            {
                pid_elapsed = current_time - last_pid_time;
                pid_dt = pid_elapsed.count()/1000.0;
            }

            if(pid_dt <= 0) pid_dt = query_rate;

            freq_adjust = fpid->calculate(freq_sp,freq,pid_dt);

            fil_curr_sp += freq_adjust; // needs scale values which are TBD
            beam_volt_sp += freq_adjust/2; // trying this

            // need to also check that voltage isnt incremented more than 100V
            if(beam_volt_sp > beam_volt_ref + 0.1)
                beam_volt_sp = beam_volt_ref + 0.1;
            else if(beam_volt_sp < beam_volt_ref - 0.1)
                beam_volt_sp = beam_volt_ref - 0.1;

            last_pid_time = current_time;
        }

        // check to make sure filament current isnt set out of bounds
        if(fil_curr_sp > 3.3) fil_curr_sp = 3.3;
        else if (fil_curr_sp < 0) fil_curr_sp = 0;

        // check to make sure beam voltage is within bounds
        if(beam_volt_sp > 5) beam_volt_sp = 5;
        else if(beam_volt_sp < 0) beam_volt_sp = 0;

        if(!relaxing && (ramping_up || ramping_down || (beam_curr_pid_on && beam_curr_sp >= 0) || (power_pid_on && power_sp >= 0) || (freq_pid_on && freq_sp > 0) || e_ramping))
        {
            stat = set_fil_curr(fil_curr_sp);
            
            if(freq_pid_on && !cath_hv_blocked && stat == 0)
                set_beam_volt(beam_volt_sp);
        }
    }
*/}

void Gyrotron::record_data()
{
    clk::time_point current_time = clk::now();
    std::chrono::duration<double, std::milli> record_elapsed = current_time - last_recording;
    double elapsed_secs = record_elapsed.count()/1000.0;

    if(elapsed_secs > rec_rate)
    {
        std::string out_str = to_str(pressure);
        log_data(out_str);
        last_recording = current_time;
    }
}

// SET functions:

int Gyrotron::set_fil_curr(double current) 
{
    if(current < 0 || current > MAX_FIL_SET)
        return -2;

    double convert = (current/MAX_FIL_SET)*4095;
    std::string cmd = "13," + to_str(int(convert)) + ",";
    std::string resp = cath.smart_io(cmd,"13,$",true);
    if(err(resp))
    {
        elog.write("failed to set filament current to " + to_str(current));
        elog.write("moving cathode to reconnect loop");
        return -1;
    }
    else
        elog.write("set filament current to " + to_str(current));
    return 0;
}

int Gyrotron::set_beam_volt(double voltage) 
{
    if(voltage < 0 || voltage > MAX_BEAM_VOLT)
        return -2;

    double convert = (voltage/MAX_BEAM_VOLT)*4095;
    std::string cmd = "10," + to_str(int(convert)) + ",";
    std::string resp = cath.smart_io(cmd,"10,$",true);
    if(err(resp))
    {
        elog.write("failed to set beam voltage to " + to_str(voltage));
        elog.write("moving cathode to reconnect loop");
        return -1;
    }
    else
        elog.write("set filament current to " + to_str(voltage));
    return 0;
}

int Gyrotron::set_gtc_curr(double current) 
{
    if(current < 0 || current > MAX_GTC_VOLT)
        return -2;

    std::string cmd = ":SOURCE1:CURR " + to_str(current);
    std::string resp = gtc.smart_write(cmd,":SOURCE1:CURR?");
    if(err(resp))
    {
        elog.write("failed to set GTC current to " + to_str(current));
        elog.write("moving GTC to reconnect loop");
        return -1;
    }
    else
    {
        if(approx_equal(to_doub(resp),current))
            elog.write("set GTC current to " + to_str(current));
        else
        {
            elog.write("ERROR GTC current setpoint =/= applied value (set: " + 
                        to_str(current) + ", got: " + resp + ")");
            shout("ERROR! GTC current setpoint =/= applied value");
            return -3;
        }
        
    }
    return 0;
}

int Gyrotron::set_gtc_volt(double voltage) 
{
    if(voltage < 0 || voltage > MAX_GTC_VOLT)
        return -2;

    std::string cmd = ":SOURCE1:VOLT " + to_str(voltage);
    std::string resp = gtc.smart_write(cmd,":SOURCE1:VOLT?");
    if(err(resp))
    {
        elog.write("failed to set GTC voltage to " + to_str(voltage));
        elog.write("moving GTC to reconnect loop");
        return -1;
    }
    else
    {
        if(approx_equal(to_doub(resp),voltage))
            elog.write("set GTC voltage to " + to_str(voltage));
        else
        {
            elog.write("ERROR GTC voltage setpoint =/= applied value (set: " + 
                        to_str(voltage) + ", got: " + resp + ")");
            shout("ERROR! GTC voltage setpoint =/= applied value");
            return -3;
        }
        
    }
    return 0;
}

int Gyrotron::set_fil_curr_limit(double current) // cathode
{
    if(current < 0 || current > MAX_FIL_CURR)
        return -2;
    
    double convert = (current/MAX_FIL_CURR)*4095;
    std::string cmd = "12," + to_str(int(convert)) + ",";
    std::string resp = cath.smart_io(cmd,"12,$",true);
    if(err(resp))
    {
        elog.write("failed to set filament current limit to " + to_str(current));
        elog.write("moving cathode to reconnect loop");
        return -1;
    }
    else
        elog.write("set filament current limit to " + to_str(current));
    return 0;
}

int Gyrotron::set_beam_curr_limit(double current)
{
    if(current <= 0 || current > 30)
        return -2;
    else
    {
        beam_curr_limit = current;
        return apply_beam_curr_limit(beam_curr_limit);
    }
}

int Gyrotron::apply_beam_curr_limit(double current)
{
    if(current < 0 || current > MAX_BEAM_CURR)
        return -2;
    
    double convert = (current/MAX_BEAM_CURR)*4095;
    std::string cmd = "11," + to_str(int(convert)) + ",";
    std::string resp = cath.smart_io(cmd,"11,$",true);
    if(err(resp))
    {
        elog.write("failed to set beam current to " + to_str(current));
        elog.write("moving cathode to reconnect loop");
        return -1;
    }
    else
        elog.write("set beam current to " + to_str(current));
    return 0;
}

int Gyrotron::set_gtc_curr_limit(double current)
{
    if(current < 0 || current > MAX_GTC_CURR)
        return -2;

    std::string cmd = ":OUTP:OCP:VAL CH1," + to_str(current);
    std::string resp = gtc.smart_write(cmd,":OUTP:OCP:VAL? CH1");
    if(err(resp))
    {
        elog.write("failed to set GTC OCP to " + to_str(current));
        elog.write("moving GTC to reconnect loop");
        return -1;
    }
    else
    {
        if(approx_equal(to_doub(resp),current))
            elog.write("set GTC OCP to " + to_str(current));
        else
        {
            elog.write("ERROR GTC OCP setpoint =/= applied value (set: " + 
                        to_str(current) + ", got: " + resp + ")");
            shout("ERROR! GTC OCP setpoint =/= applied value");
            return -3;
        }
        
    }
    return 0;
}

int Gyrotron::set_gtc_volt_limit(double voltage)
{
    if(voltage < 0 || voltage > MAX_GTC_VOLT)
        return -2;

    std::string cmd = ":OUTP:OVP:VAL CH1," + to_str(voltage);
    std::string resp = gtc.smart_write(cmd,":OUTP:OVP:VAL? CH1");
    if(err(resp))
    {
        elog.write("failed to set GTC OVP to " + to_str(voltage));
        elog.write("moving GTC to reconnect loop");
        return -1;
    }
    else
    {
        if(approx_equal(to_doub(resp),voltage))
            elog.write("set GTC OVP to " + to_str(voltage));
        else
        {
            elog.write("ERROR GTC OVP setpoint =/= applied value (set: " + 
                        to_str(voltage) + ", got: " + resp + ")");
            shout("ERROR! GTC OVP setpoint =/= applied value");
            return -3;
        }
        
    }
    return 0;
}

void set_beam_kp(double kp) { 
    //beam_kp = kp; beam_pid = new PID(beam_kp, beam_ki, beam_kd);
    //log_event("beam PID constants updated (" + to_str(beam_kp) + "," + to_str(beam_ki) + "," + to_str(beam_kd) + ")");
}
void set_beam_ki(double ki) {
    //beam_ki = ki; beam_pid = new PID(beam_kp, beam_ki, beam_kd);
    //log_event("beam PID constants updated (" + to_str(beam_kp) + "," + to_str(beam_ki) + "," + to_str(beam_kd) + ")");
}
void set_beam_kd(double kd) {
    //beam_kd = kd; beam_pid = new PID(beam_kp, beam_ki, beam_kd);
    //log_event("beam PID constants updated (" + to_str(beam_kp) + "," + to_str(beam_ki) + "," + to_str(beam_kd) + ")");
}
void set_power_kp(double kp) {
    //power_kp = kp; power_pid = new PID(power_kp, power_ki, power_kd);
    //log_event("power PID constants updated (" + to_str(power_kp) + "," + to_str(power_ki) + "," + to_str(power_kd) + ")");
}
void set_power_ki(double ki) {
    //power_ki = ki; power_pid = new PID(power_kp, power_ki, power_kd);
    //log_event("power PID constants updated (" + to_str(power_kp) + "," + to_str(power_ki) + "," + to_str(power_kd) + ")");
}
void set_power_kd(double kd) {
    //power_kd = kd; power_pid = new PID(power_kp, power_ki, power_kd);
    //log_event("power PID constants updated (" + to_str(power_kp) + "," + to_str(power_ki) + "," + to_str(power_kd) + ")");
}
void set_freq_kp(double kp) {
    //freq_kp = kp; freq_pid = new PID(freq_kp, freq_ki, freq_kd);
    //log_event("frequency PID constants updated (" + to_str(freq_kp) + "," + to_str(freq_ki) + "," + to_str(freq_kd) + ")");
}
void set_freq_ki(double ki) {
    //freq_ki = ki; freq_pid = new PID(freq_kp, freq_ki, freq_kd);
    //log_event("frequency PID constants updated (" + to_str(freq_kp) + "," + to_str(freq_ki) + "," + to_str(freq_kd) + ")");
}
void set_freq_kd(double kd) {
    //freq_kd = kd; freq_pid = new PID(freq_kp, freq_ki, freq_kd);
    //log_event("frequency PID constants updated (" + to_str(freq_kp) + "," + to_str(freq_ki) + "," + to_str(freq_kd) + ")");
}

int Gyrotron::toggle_cath_output(bool turn_on)
{/*
    std::string cmd = "98," + std::string(turn_on ? "1," : "0,");
    std::string resp = cath.smart_io(cmd,"98,$",true);
    if(err(resp))
    {
        elog.write("failed to " + std::string(turn_on ? "enable" : "disable") + " cathode output");
        elog.write("moving cathode to reconnect loop");
        return -1;
    }
    elog.write(std::string(turn_on ? "enabled" : "disabled") + " cathode output");
    cath_hv_on = turn_on;*/
    return 0;
}

int Gyrotron::toggle_gtc_output(bool turn_on)
{
    std::string cmd = ":OUTP CH1," + std::string(turn_on ? "ON" : "OFF");
    std::string resp = gtc.smart_write(cmd,":OUTP? CH1","ON",turn_on);
    if(err(resp))
    {
        elog.write("failed to " + std::string(turn_on ? "enable" : "disable") + " GTC output");
        elog.write("moving GTC to reconnect loop");
        return -1;
    }
    elog.write(std::string(turn_on ? "enabled" : "disabled") + " GTC output");
    gtc_hv_on = turn_on;
    return 0;
}

void Gyrotron::toggle_beam_pid() { toggle_beam_pid(!beam_pid_on); }
void Gyrotron::toggle_power_pid() { toggle_power_pid(!power_pid_on); } 
void Gyrotron::toggle_freq_pid() { toggle_freq_pid(!freq_pid_on); } 

void Gyrotron::toggle_beam_pid(bool turn_on)
{/*
    if(turn_on)
    {
        freq_pid_on = power_pid_on = false;
        beam_curr_sp = beam_curr;
        beam_sp_data.push_back(beam_curr_sp);
        beam_time_data.push_back(runtime());
        beam_pid = new PID(beam_kp, beam_ki, beam_kd); // re-initialize PID controller
        reset_pid_time = true; // FIX THIS
    } 
    beam_pid_on = turn_on;
    log_event("automatic beam control " + std::string(turn_on ? "enabled" : "disabled"));
*/}

void Gyrotron::toggle_power_pid(bool turn_on)
{/*
    if(turn_on)
    {
        freq_pid_on = beam_pid_on = false;
        power_sp = power;
        power_sp_data.push_back(power_sp);
        power_time_data.push_back(runtime());
        power_pid = new PID(power_kp, power_ki, power_kd); // re-initialize PID controller
        reset_pid_time = true;
    }
    power_pid_on = turn_on;
    log_event("automatic power control " + std::string(turn_on ? "enabled" : "disabled"));
*/}

void Gyrotron::toggle_freq_pid(bool turn_on)
{/*
    if(turn_on)
    {
        beam_pid_on = power_pid_on = false;
        freq_sp = freq;
        beam_volt_ref = beam_volt;
        freq_pid = new PID(freq_kp, freq_ki, freq_kd); // re-initialize PID controller
        reset_pid_time = true;
    }
    freq_pid_on = turn_on;
    log_event("automatic frequency control " + std::string(turn_on ? "enabled" : "disabled"));
*/}

int enter_standby()
{/*
    beam_pid_on = power_pid_on = freq_pid_on = false;
    ramping_up = ramping_down = false;
    int stat = toggle_cath_output(false);
    if(stat < 0) return -1;
    stat = toggle_gtc_output(false);
    if(stat < 0) return -2;
    return 0;
*/}

void Gyrotron::set_beam_curr(double c) {
    beam_curr_sp = c;
    beam_sp_data.push_back(beam_curr_sp);
    beam_time_data.push_back(runtime());
    beam_pid = new PID(beam_kp, beam_ki, beam_kd); // re-initialize PID controller
    reset_pid_time = true;
}
void Gyrotron::set_freq(double f) {
    freq_sp = f;
    freq_pid = new PID(freq_kp, freq_ki, freq_kd); // re-initialize PID controller
    reset_pid_time = true;
}
void Gyrotron::set_power(double p) {
    power_sp = p;
    power_sp_data.push_back(power_sp);
    power_time_data.push_back(runtime());
    power_pid = new PID(power_kp, power_ki, power_kd); // re-initialize PID controller
    reset_pid_time = true;
}

void Gyrotron::set_ramp_time(double time) { ramp_time = time; }
void Gyrotron::set_ramp_sp(double sp) { ramp_sp = sp; }

// status functions:

bool Gyrotron::is_ramping_down() { return ramping_down; }
bool Gyrotron::is_ramping_up() { return ramping_up; }
bool Gyrotron::cath_is_enabled() { return cath.is_enabled(); }
bool Gyrotron::gtc_is_enabled() { return gtc.is_enabled(); }
bool Gyrotron::spc_is_enabled() { return spc.is_enabled(); }
bool Gyrotron::rsi_is_enabled() { return rsi.is_enabled(); }
bool Gyrotron::fms_is_enabled() { return fms.is_enabled(); }
bool Gyrotron::beam_pid_is_on() { return beam_pid_on; }
bool Gyrotron::power_pid_is_on() { return power_pid_on; }
bool Gyrotron::freq_pid_is_on() { return freq_pid_on; }

// GET functions:

double Gyrotron::get_fil_curr_sp() { return fil_curr_sp; }
double Gyrotron::get_beam_volt_sp() { return beam_volt_sp; }
double Gyrotron::get_beam_curr_sp() { return beam_curr_sp; }
double Gyrotron::get_freq_sp() { return freq_sp; }
double Gyrotron::get_power_sp() { return power_sp; }
double Gyrotron::get_gtc_curr_sp() { return beam_curr_sp; }
double Gyrotron::get_gtc_volt_sp() { return beam_curr_sp; }
int Gyrotron::get_state() { /*return state;*/ return 0; }
double Gyrotron::get_fil_curr() { return fil_curr; }
double Gyrotron::get_beam_volt() { return beam_volt; }
double Gyrotron::get_beam_curr() { return beam_curr; }
double Gyrotron::get_collector_curr() { /*return collector_curr;*/return 0; }
double Gyrotron::get_body_curr() { /*return body_curr;*/return 0; }
double Gyrotron::get_freq() { return freq; }
double Gyrotron::get_power() { return power; }
// 0 = safe temp, -1 = warning, -2 = fatal
int Gyrotron::get_temp_status() { /*return temp_stat;*/return 0; }
// 0 = safe temp, -1 = warning, -2 = fatal
int Gyrotron::get_flow_status() { /*return flow_stat;*/return 0; }
double Gyrotron::get_temp() { /*return temp;*/return 0; }
double Gyrotron::get_flow() { /*return flow;*/return 0; }
double Gyrotron::get_pressure() { /*return press;*/return 0; }
double Gyrotron::get_gtc_curr() { return gtc_curr; }
double Gyrotron::get_gtc_volt() { return gtc_volt; }
double Gyrotron::get_ramp_time() { return ramp_time; }
double Gyrotron::get_ramp_sp() { return ramp_sp; }

double Gyrotron::runtime()
{
    clk::time_point current_time = clk::now();
    std::chrono::duration<double, std::milli> elapsed_runtime = current_time - get_start_time();
    return elapsed_runtime.count()/1000.0;
}

int Gyrotron::increment_state()
{
    if(current_state == 3)
        return -1;
    
    switch(current_state)
    {
    case 0: 
        if(prepare_all() < 0) { return -2; } ramping_up = true; break;
    case 1: 
        if(ramping_down && !paused) { paused = true; }
        else if(ramping_down && paused) { ramping_down = false; ramping_up = true; paused = false; }
        else if(ramping_up && paused) { paused = false; }
        break;
    case 2: if(toggle_mw(true) < 0) { return -2; } break;
    }
    return 0;
}

int Gyrotron::decrement_state()
{
    if(current_state == 0)
        return -1;
    
    switch(current_state)
    {
    case 1: if(ramping_up && !paused) { paused = true; }
        else if(ramping_up && paused) { ramping_up = false; ramping_down = true; paused = false; }
        else if(ramping_down && paused) { paused = false; }
        break;
    case 2: ramping_down = true; break;
    case 3: if(toggle_mw(false) < 0) { return -2; } break;
    }
    return 0;
}

int Gyrotron::toggle_mw(bool turn_on)
{
    if(turn_on)
    {
        if(toggle_cath_output(true) < 0 || toggle_gtc_output(true) < 0)
        {
            toggle_cath_output(false);
            toggle_gtc_output(false);
            return -1;
        }
    }
    else if(toggle_cath_output(false) < 0 || toggle_gtc_output(false) < 0)
        return -1;
    
    return 0;
}

int Gyrotron::prepare_all()
{
    if(set_beam_volt(OP_BEAM_VOLT) < 0 || set_gtc_volt(OP_GTC_VOLT) < 0 || set_gtc_curr(OP_GTC_CURR) < 0)
        return -1;
    return 0;
}

bool Gyrotron::all_clear()
{
    if(cath.is_enabled() && gtc.is_enabled() && spc.is_enabled() &&
        rsi.is_enabled() && fms.is_enabled() &&
        cath.is_connected() && gtc.is_connected() && spc.is_connected() &&
        rsi.is_connected() && fms.is_connected() && get_sys_errors().empty())
    { return true; }

    return false;
}
