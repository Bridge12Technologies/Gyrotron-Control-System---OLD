#include "gyrotron.h"

Gyrotron::Gyrotron() : Spine()
{
    elog.set_history_enabled(true);
    dlog.set_header("system time (H:M:S), pressure (Torr), etc etc");

    add_device(&cath,[=](){query_cath();},[=](){cath_recon();},"99,1,","$");
    add_device(&gtc,[=](){query_gtc();},[=](){gtc_recon();},"*IDN?","DP821A");
    add_device(&rsi,[=](){query_rsi();},[=](){rsi_recon();},"ping","OK");
    add_device(&spc,[=](){query_spc();},[=](){spc_recon();},"spc 01","00");
    add_device(&fms,[=](){query_fms();},[=](){fms_recon();},"#0","0");

    add_pre_func([=](){apply_beam_curr_limit(BEAM_CURR_LIMIT);});
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
    std::string temp;

    std::string titles[19] = {"Current PID Proportional","Current PID Integral","Current PID Derivative",
                            "Power PID Proportional","Power PID Integral","Power PID Derivative",
                            "Frequency PID Proportional","Frequency PID Integral","Frequency PID Derivative",
                            "Filament Ramp Set Point","Normal Ramp Time","Emergency Ramp Time","Fatal Pressure Limit",
                            "Filament Pressure High Limit","Filament Pressure Low Limit","HV Pressure Low Limit",
                            "Power Meter Calibration", "Error Count First Limit","Error Count Second Limit"};
    std::atomic<double> *a_values[12] = {&beam_kp,&beam_ki,&beam_kd,&power_kp,&power_ki,&power_kd,
                                         &freq_kp,&freq_ki,&freq_kd,&ramp_sp,&ramp_time,&e_ramp_time};
    double *values[5] = {&fatal_press,&press_bound3,&press_bound2,&press_bound1,&power_calibrate};

    for(i = 0; i < 12; i++) { *a_values[i] = config_get_double(titles[i],ok); }
    for(i = 0; i < 5; i++) { *values[i] = config_get_double(titles[i+12],ok); }

    std::string ser_titles[]{"Cathode Port","RSI Port"};
    Device* ser_devs[]{&cath,&rsi};
    std::string eth_titles[]{"GTC IP","SPC IP","FMS IP"};
    Device* eth_devs[]{&gtc,&spc,&fms};

    i = 0;
    for(std::string title : ser_titles)
    {
        temp = config_get_str(title,ok);
        if(temp == "x" || temp == "X")
            ser_devs[i]->set_enabled(false);
        else
            ser_devs[i]->set_com_port("/dev/tty" + temp);
        i++;
    }
    i = 0;
    for(std::string title : eth_titles)
    {
        temp = config_get_str(title,ok);
        if(temp == "x" || temp == "X")
            eth_devs[i]->set_enabled(false);
        else
            eth_devs[i]->set_ip(temp);
        i++;
    }

    set_log_in_bin(config_get_bool("Record Data in Binary Format",ok));

    if(!ok) return -1;

    ramp_time = ramp_time * 60; // convert to seconds
    e_ramp_time = e_ramp_time * 60;
    ramp_rate = (ramp_sp/ramp_time)*query_rate; // calculate ramp rate from time and sp
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
    std::string resp = cath.m_smart_io("19,","19,",true);
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

        beam_volt_m.lock();
        beam_volt_time_data.push_back(runtime());
        beam_volt_data.push_back(beam_volt);
        if(beam_volt_sp > 0)
            beam_volt_sp_data.push_back(beam_volt_sp);
        else
            beam_volt_sp_data.push_back(std::numeric_limits<double>::quiet_NaN());

        while(runtime() - *beam_volt_time_data.begin() > plot_span)
        {
            if(!beam_volt_data.empty()) beam_volt_data.erase(beam_volt_data.begin());
            if(!beam_volt_sp_data.empty()) beam_volt_sp_data.erase(beam_volt_sp_data.begin());
            if(!beam_volt_time_data.empty()) beam_volt_time_data.erase(beam_volt_time_data.begin());
        }
        beam_volt_m.unlock();

        fil_curr_m.lock();
        fil_curr_time_data.push_back(runtime());
        fil_curr_data.push_back(fil_curr);
        if(fil_curr_sp > 0)
            fil_curr_sp_data.push_back(fil_curr_sp);
        else
            fil_curr_sp_data.push_back(std::numeric_limits<double>::quiet_NaN());

        while(runtime() - *fil_curr_time_data.begin() > plot_span)
        {
            if(!fil_curr_data.empty()) fil_curr_data.erase(fil_curr_data.begin());
            if(!fil_curr_sp_data.empty()) fil_curr_sp_data.erase(fil_curr_sp_data.begin());
            if(!fil_curr_time_data.empty()) fil_curr_time_data.erase(fil_curr_time_data.begin());
        }
        fil_curr_m.unlock();

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
            if(int(temp_int) == 1) cath.push_error("arc detected");
            ss >> temp_int; ss >> comma;
            if(int(temp_int) == 1) cath.push_error("over temperature");
            ss >> temp_int; ss >> comma;
            if(int(temp_int) == 1) cath.push_warning("over voltage");
            ss >> temp_int; ss >> comma;
            if(int(temp_int) == 1) cath.push_warning("under voltage");
            ss >> temp_int; ss >> comma;
            if(int(temp_int) == 1) cath.push_warning("over current");
            ss >> temp_int;
            if(int(temp_int) == 1) cath.push_warning("under current");

        }
        else if(err(resp))
            log_event("Cathode D/C, moving to reconnect loop");
        
        cath.warning_m()->unlock(); 
        cath.error_m()->unlock();
        beam_curr_m.lock();

        beam_curr_time_data.push_back(runtime());
        beam_curr_data.push_back(beam_curr);
        if(beam_pid_on)
            beam_curr_sp_data.push_back(beam_curr_sp);
        else
            beam_curr_sp_data.push_back(std::numeric_limits<double>::quiet_NaN());

        while(runtime() - *beam_curr_time_data.begin() > plot_span)
        {
            if(!beam_curr_data.empty()) beam_curr_data.erase(beam_curr_data.begin());
            if(!beam_curr_sp_data.empty()) beam_curr_sp_data.erase(beam_curr_sp_data.begin());
            if(!beam_curr_time_data.empty()) beam_curr_time_data.erase(beam_curr_time_data.begin());
        }
        beam_curr_m.unlock();
        steer_cath();
    }
    else  
        log_event("Cathode D/C, moving to reconnect loop");
}

void Gyrotron::query_gtc() // still need to check responses manually and apply to error checks
{
    std::stringstream ss;
    double temp_double;

    std::string resp = gtc.m_smart_io("VREAD","error",false);
    if(!err(resp))
    {
        temp_double = to_doub(resp);
        if(temp_double >= 0) // check conversion
            gtc_volt = temp_double;
        else
            log_event(shout("invalid gtc voltage conversion: " + resp));
        
        resp = gtc.m_smart_io("IREAD","error",false);
        if(!err(resp))
        {
            temp_double = to_doub(resp);
            if(temp_double >= 0) // check conversion
                gtc_curr = temp_double;
            else
                log_event(shout("invalid gtc current conversion: " + resp));

            resp = gtc.m_smart_io("PWR");
            if(!err(resp))
            {
                if(contains(resp,"ON"))
                    gtc_hv_on = true;
                else if(contains(resp,"OFF"))
                    gtc_hv_on = false;
                else
                    log_event("couldn't read GTC HV status: " + resp);
                
                resp = gtc.m_smart_io(":SYST:ERR:COUNT?");
                if(contains(resp,"0"))
                {
                    gtc.error_m()->lock();
                    gtc.clear_errors();
                    gtc.error_m()->unlock();
                }
                else
                {
                    resp = gtc.m_smart_io(":SYST:ERR:ALL?");
                    if(!err(resp))
                    {
                        gtc.error_m()->lock();
                        //gtc.warning_m()->lock();
                        gtc.clear_errors();
                        //gtc.clear_warnings();

                        if(contains(resp,"-"))
                            gtc.push_error(resp);

                        //gtc.warning_m()->unlock();
                        gtc.error_m()->unlock();
                    }
                    else
                        log_event("GTC D/C, moving to reconnect loop");
                }

            }
            else
                log_event("GTC D/C, moving to reconnect loop");
        }
        else
            log_event("GTC D/C, moving to reconnect loop");
    }
    else
        log_event("GTC D/C, moving to reconnect loop");
}

void Gyrotron::query_spc()
{
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
            log_event(shout("invalid pressure conversion: " + resp));

        if(pressure >= fatal_press && !e_ramping)
        {
            e_ramping = true;
            log_event("hit fatal pressure limit, begin emergency ramp down");
        }
        if(pressure >= press_bound3 && !relaxing)
        {
            relaxing = true;
            log_event("hit pressure high limit for filament, begin relaxation state");
        }
        else if(pressure <= press_bound2 && relaxing)
        {
            relaxing = false;
            log_event("hit pressure low limit for filament, end relaxation state");
        }

        if(pressure >= press_bound2 && !hv_blocked && cath_hv_on)
        {
            toggle_cath_output(false);
            cath_hv_on_prev.store(cath_hv_on);
            cath_hv_on = false;
            hv_blocked = true;
            
            freq_pid_on_prev.store(freq_pid_on);
            freq_pid_on = false;
            power_pid_on_prev.store(power_pid_on);
            power_pid_on = false;
            beam_pid_on_prev.store(beam_pid_on);
            beam_pid_on = false;
            
            log_event("hit pressure high limit for HV, begin blocking HV");
        }
        else if(pressure <= press_bound1 && hv_blocked)
        {
            hv_blocked = false;
            if(cath_hv_on_prev) { if(toggle_cath_output(true) >= 0) { cath_hv_on = true; }}

            freq_pid_on.store(freq_pid_on_prev);
            power_pid_on.store(power_pid_on_prev);
            beam_pid_on.store(beam_pid_on_prev);

            log_event("hit pressure low limit for HV, stop blocking HV");
        }
        press_m.lock();
        // add in plotting vector management here
        press_time_data.push_back(runtime());
        press_data.push_back(pressure);

        while(runtime() - *press_time_data.begin() > plot_span)
        {
            if(!press_data.empty()) press_data.erase(press_data.begin());
            if(!press_time_data.empty()) press_time_data.erase(press_time_data.begin());
        }
        press_m.unlock();
    }
    else
        log_event("SPC D/C, moving to reconnect loop");
}

void Gyrotron::query_fms()
{
    double temp_double;

    std::string resp = fms.m_smart_io("#F","#F OK",true);
    if(!err(resp))
    {
        resp.erase(0,6); // erase "#2 OK "
        temp_double = to_doub(resp);
        if(temp_double > 0)
        {
            freq = temp_double;

            freq_m.lock();
            freq_time_data.push_back(runtime());
            freq_data.push_back(freq);
            if(freq_pid_on)
                freq_sp_data.push_back(freq_sp);
            else
                freq_sp_data.push_back(std::numeric_limits<double>::quiet_NaN());

            while(runtime() - *freq_time_data.begin() > plot_span)
            {
                if(!freq_data.empty()) freq_data.erase(freq_data.begin());
                if(!freq_sp_data.empty()) freq_sp_data.erase(freq_sp_data.begin());
                if(!freq_time_data.empty()) freq_time_data.erase(freq_time_data.begin());
            }
            freq_m.unlock();
        }
        else
            log_event("FMS bad conversion: " + resp);
    }
    else
        log_event("FMS D/C, moving to reconnect loop");
}

void Gyrotron::query_rsi()
{
    std::string cmds[] = {"mpio 1 ar 1","mpio 1 ar 2","mpio 1 ar 3","temp 1","temp 2","temp 3",
                          "temp 4","temp 5","flow 1","flow 2","flow 3","flow 4"};
    std::atomic<double> *values[] = {&diode_volt, &body_volt, &collector_volt,
                                     &main_chill_temp, &cav_chill_temp, &collector_temp, &cav_temp, &body_temp,
                                     &main_chill_flow, &cav_chill_flow, &collector_flow, &gun_air_flow};
    std::string resp;
    int stat = 0, i;

    rsi.m()->lock();
    for(i = 0; i < 12; i++)
    {
        if(stat == 0)
        {
            resp = rsi.smart_io(cmds[i]);
            if(!err(resp)) values[i]->store(to_doub(resp));
            else stat = -1;
        }
        if(stat < 0) break;
    }
    rsi.m()->unlock();

    if(stat < 0)
    {
        rsi.error_m()->lock();
        rsi.clear_errors();
        rsi.error_m()->unlock();
        log_event("SPC D/C, moving to reconnect loop");
    }
    else
    {
        rsi.error_m()->lock();
        rsi.clear_errors();
        rsi.error_m()->unlock();

        if(diode_volt >= 0)
            power = diode_volt * POWER_V_CONVERT;
        if(body_volt >= 0)
            body_curr = body_volt/BODY_R;
        if(collector_volt >= 0)
            collector_curr = collector_volt/COLLECTOR_R;

        power_m.lock();
        // add in plotting vector management here
        power_time_data.push_back(runtime());
        power_data.push_back(power);
        if(power_pid_on)
            power_sp_data.push_back(power_sp);
        else
            power_sp_data.push_back(std::numeric_limits<double>::quiet_NaN());

        while(runtime() - *power_time_data.begin() > plot_span)
        {
            if(!power_data.empty()) power_data.erase(power_data.begin());
            if(!power_sp_data.empty()) power_sp_data.erase(power_sp_data.begin());
            if(!power_time_data.empty()) power_time_data.erase(power_time_data.begin());
        }
        power_m.unlock();
    }
}

void Gyrotron::eval_sys_stat()
{
    Device *devs[] = {&cath,&gtc,&spc,&rsi,&fms};

    sys_error_m()->lock();
    sys_warning_m()->lock();
    clear_sys_errors();
    clear_sys_warnings();

    if(hv_blocked)
        push_sys_warning("HV blocked until pressure lowers");

    if(relaxing && !e_ramping)
        push_sys_warning("hit high pressure limit, relaxation in progress");

    if(e_ramping)
        push_sys_error("hit fatal pressure limit, emergency ramp down in progress");

    for(Device *dev : devs) {
        if(dev->is_enabled() && !(dev->is_connected()))
            push_sys_error(dev->get_name() + " is disconnected! Reconnection attempts in progresss...");
    }

    sys_error_m()->unlock();
    sys_warning_m()->unlock();
}

std::vector<std::string> Gyrotron::get_warnings()
{
    std::vector<std::string> warnings, temp;

    // collect system warnings
    warnings = get_sys_warnings();
    // collect cathode warnings
    if(cath.is_enabled() && cath.is_connected())
    {
        temp = cath.m_get_warnings();
        warnings.insert(warnings.end(), temp.begin(), temp.end());
    }
    // collect GTC warnings
    //temp = gtc.m_get_warnings();
    //warnings.insert(warnings.end(), temp.begin(), temp.end());

    return warnings;
}

std::vector<std::string> Gyrotron::get_errors()
{
    std::vector<std::string> errors, temp;

    // collect system errors
    errors = get_sys_errors(!gui_debug_mode);
    // collect cathode errors
    if(cath.is_enabled() && cath.is_connected())
    {
        temp = cath.m_get_errors();
        errors.insert(errors.end(), temp.begin(), temp.end());
    }

    // collect gtc errors
    if(gtc.is_enabled() && gtc.is_connected())
    {
        temp = gtc.m_get_errors();
        errors.insert(errors.end(), temp.begin(), temp.end());
    }

    return errors;
}

void Gyrotron::steer_cath()
{
    int stat;
    clk::time_point current_time = clk::now();
    
    bool adjust_needed = (ramping_up || ramping_down || e_ramping || // ramp in progress
                          (beam_pid_on && beam_curr_sp >= 0) || // beam current PID active
                          (power_pid_on && power_sp >= 0) || // power PID active
                          (freq_pid_on && freq_sp >= 0)); // frequency PID active

    if(cath.is_connected() && adjust_needed)
    {
        if(e_ramping)
        {
            fil_curr_sp = fil_curr_sp - e_ramp_rate;
            if(fil_curr_sp <= 0) // check if finished
            {
                e_ramping = false;
                current_state = 0;
                log_event("emergency ramp down finished");
            }
        }
        else if(ramping_up && !relaxing)
        {
            fil_curr_sp = fil_curr_sp + ramp_rate; // increment filament current
            if(fil_curr_sp >= ramp_sp) // check if finished
            {
                ramping_up = false;
                current_state = 2;
                log_event("filament ramp up finished");
            }
        }
        else if (ramping_down && !relaxing)
        {
            fil_curr_sp = fil_curr_sp - ramp_rate; // decrement filament current
            if(fil_curr_sp <= 0) // check if finished
            {
                ramping_down = false;
                current_state = 0;
                log_event("filament ramp down finished");
            }
        }
        else if(beam_pid_on && beam_curr_sp >= 0 && !relaxing && !hv_blocked) // beam current PID control
        {
            if(reset_pid_time) // set default dt if pid control is just starting/resuming
            {
                pid_dt = query_rate;
                fil_curr_sp.store(fil_curr);
                reset_pid_time = false;
            }
            else // otherwise calculate the dt precisely
            {
                pid_elapsed = current_time - last_pid_time;
                pid_dt = pid_elapsed.count()/1000.0;
            }

            if(pid_dt <= 0) pid_dt = query_rate;

            // calculate neccisary adjustment from PID calculation
            double beam_curr_adjust = beam_pid->calculate(beam_curr_sp,beam_curr,pid_dt);
            //fil_curr_sp += (double)(beam_curr_adjust * beam2fil_scale); // scale the adjustment to beam current for filament

            // be careful that this adjustment isn't problematic, especially prior to proper tuning
            if((beam_curr+beam_curr_adjust) > 30) { beam_curr_adjust = 30 - beam_curr; }
            else if((beam_curr+beam_curr_adjust) < 0) { beam_curr_adjust = beam_curr * (-1); }

            fil_curr_sp = fil_curr_sp + beam_curr_adjust;

            last_pid_time = current_time;
        }
        else if(power_pid_on && power_sp >= 0 && power_data.size() >= 3 && !relaxing && !hv_blocked) // power PID control
        {
            if(reset_pid_time) // set default dt if pid control is just starting/resuming
            {
                pid_dt = query_rate;
                fil_curr_sp.store(fil_curr);
                reset_pid_time = false;
            }
            else // otherwise calculate the dt precisely
            {
                pid_elapsed = current_time - last_pid_time;
                pid_dt = pid_elapsed.count()/1000.0;
            }

            if(pid_dt <= 0) pid_dt = query_rate;

            // calculate neccisary adjustment from PID calculation
            double power_adjust = power_pid->calculate(power_sp,power,pid_dt);
            //double power_adjust = power_pid->calculate(power_sp,power_avg,dt);

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
            fil_curr_sp = fil_curr_sp + (power_adjust * 15); // trying out a scale to keep roughly the same PID constants for both controllers

            last_pid_time = current_time;
        }
        else if (freq_pid_on && freq_sp > 0 && !relaxing && !hv_blocked)
        {
            if(reset_pid_time) // set default dt if pid control is just starting/resuming
            {
                pid_dt = query_rate;
                fil_curr_sp.store(fil_curr);
                beam_volt_sp.store(beam_volt);
                reset_pid_time = false;
            }
            else // otherwise calculate the dt precisely
            {
                pid_elapsed = current_time - last_pid_time;
                pid_dt = pid_elapsed.count()/1000.0;
            }

            if(pid_dt <= 0) pid_dt = query_rate;

            double freq_adjust = freq_pid->calculate(freq_sp,freq,pid_dt);

            fil_curr_sp = fil_curr_sp + freq_adjust; // needs scale values which are TBD
            beam_volt_sp = beam_volt_sp + (freq_adjust/2); // trying this

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

        if(!relaxing && (ramping_up || ramping_down || (beam_pid_on && beam_curr_sp >= 0) || (power_pid_on && power_sp >= 0) || (freq_pid_on && freq_sp > 0) || e_ramping))
        {
            stat = set_fil_curr(fil_curr_sp);
            
            if(freq_pid_on && !hv_blocked && stat == 0)
                set_beam_volt(beam_volt_sp);
        }
    }
}

void Gyrotron::record_data()
{
    clk::time_point current_time = clk::now();
    std::chrono::duration<double, std::milli> record_elapsed = current_time - last_recording;
    double elapsed_secs = record_elapsed.count()/1000.0;
    std::string out_str = "";

    // output (in order):
    // abs. time, pressure, beam curr, beam volt, fil curr, gtc curr,
    // gtc volt, collector curr, body curr, power, freq

    if(elapsed_secs > rec_rate)
    {
        if(spc.is_connected() && spc.is_enabled())
            out_str += to_str(pressure) + ",";
        else
            out_str += "NaN,";

        if(cath.is_connected() && cath.is_enabled())
            out_str += to_str(beam_curr) + "," + to_str(beam_volt) + "," + to_str(fil_curr) + ",";
        else
            out_str += "NaN,NaN,NaN,";

        if(gtc.is_connected() && gtc.is_enabled())
            out_str += to_str(gtc_curr) + "," + to_str(gtc_volt) + ",";
        else
            out_str += "NaN,NaN,";

        if(rsi.is_connected() && rsi.is_enabled())
            out_str += to_str(collector_curr) + "," + to_str(body_curr) + "," + to_str(power) + ",";
        else
            out_str += "NaN,NaN,NaN,";

        if(fms.is_connected() && fms.is_enabled())
            out_str += to_str(freq);
        else
            out_str += "NaN";

        log_data(out_str);
        last_recording = current_time;
    }
}

// SET functions:

int Gyrotron::set_fil_curr(double current) 
{
    if(current < 0 || current > FIL_CURR_LIMIT)
        return -2;

    double convert = (current/FIL_CURR_LIMIT)*4095;
    std::string cmd = "13," + to_str(int(convert)) + ",";
    std::string resp = cath.smart_io(cmd,"13,$",true);
    if(err(resp))
    {
        log_event("failed to set filament current to " + to_str(current));
        log_event("moving cathode to reconnect loop");
        return -1;
    }
    else
        log_event("set filament current to " + to_str(current));
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
        log_event("failed to set beam voltage to " + to_str(voltage));
        log_event("moving cathode to reconnect loop");
        return -1;
    }
    else
        log_event("set filament current to " + to_str(voltage));
    return 0;
}

int Gyrotron::set_gtc_volt(double voltage)
{
    if(voltage < 0 || voltage > MAX_GTC_VOLT)
        return -2;

    std::string cmd = "VSET " + to_str(voltage);
    std::string resp = gtc.smart_write(cmd,"VSET");
    if(err(resp))
    {
        log_event("failed to set GTC voltage to " + to_str(voltage));
        log_event("moving GTC to reconnect loop");
        return -1;
    }
    else
    {
        if(approx_equal(to_doub(resp),voltage))
            log_event("set GTC voltage to " + to_str(voltage));
        else
        {
            log_event("ERROR GTC voltage setpoint =/= applied value (set: " +
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
        log_event("failed to set filament current limit to " + to_str(current));
        log_event("moving cathode to reconnect loop");
        return -1;
    }
    else
        log_event("set filament current limit to " + to_str(current));
    return 0;
}

int Gyrotron::set_beam_curr_limit(double current)
{
    if(current <= 0 || current > 30)
        return -2;
    else
    {
        beam_ocp = current;
        return apply_beam_curr_limit(beam_ocp);
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
        log_event("failed to set beam current to " + to_str(current));
        log_event("moving cathode to reconnect loop");
        return -1;
    }
    else
        log_event("set beam current to " + to_str(current));
    return 0;
}

int Gyrotron::set_gtc_curr_limit(double current) // need to manually check output for this cmd
{
    if(current < 0 || current > MAX_GTC_CURR)
        return -2;

    std::string cmd = "ISET " + to_str(current);
    std::string resp = gtc.m_smart_write(cmd,"ISET");
    if(err(resp))
    {
        log_event("failed to set GTC OCP to " + to_str(current));
        log_event("moving GTC to reconnect loop");
        return -1;
    }
    else
    {
        if(approx_equal(to_doub(resp),current))
            log_event("set GTC OCP to " + to_str(current));
        else
        {
            log_event("ERROR GTC OCP setpoint =/= applied value (set: " +
                        to_str(current) + ", got: " + resp + ")");
            shout("ERROR! GTC OCP setpoint =/= applied value");
            return -3;
        }
        
    }
    return 0;
}

void Gyrotron::set_beam_kp(double kp) {
    beam_kp = kp; beam_pid = new PID(beam_kp, beam_ki, beam_kd);
    log_event("beam PID constants updated (" + to_str(beam_kp) + "," + to_str(beam_ki) + "," + to_str(beam_kd) + ")");
}
void Gyrotron::set_beam_ki(double ki) {
    beam_ki = ki; beam_pid = new PID(beam_kp, beam_ki, beam_kd);
    log_event("beam PID constants updated (" + to_str(beam_kp) + "," + to_str(beam_ki) + "," + to_str(beam_kd) + ")");
}
void Gyrotron::set_beam_kd(double kd) {
    beam_kd = kd; beam_pid = new PID(beam_kp, beam_ki, beam_kd);
    log_event("beam PID constants updated (" + to_str(beam_kp) + "," + to_str(beam_ki) + "," + to_str(beam_kd) + ")");
}
void Gyrotron::set_power_kp(double kp) {
    power_kp = kp; power_pid = new PID(power_kp, power_ki, power_kd);
    log_event("power PID constants updated (" + to_str(power_kp) + "," + to_str(power_ki) + "," + to_str(power_kd) + ")");
}
void Gyrotron::set_power_ki(double ki) {
    power_ki = ki; power_pid = new PID(power_kp, power_ki, power_kd);
    log_event("power PID constants updated (" + to_str(power_kp) + "," + to_str(power_ki) + "," + to_str(power_kd) + ")");
}
void Gyrotron::set_power_kd(double kd) {
    power_kd = kd; power_pid = new PID(power_kp, power_ki, power_kd);
    log_event("power PID constants updated (" + to_str(power_kp) + "," + to_str(power_ki) + "," + to_str(power_kd) + ")");
}
void Gyrotron::set_freq_kp(double kp) {
    freq_kp = kp; freq_pid = new PID(freq_kp, freq_ki, freq_kd);
    log_event("frequency PID constants updated (" + to_str(freq_kp) + "," + to_str(freq_ki) + "," + to_str(freq_kd) + ")");
}
void Gyrotron::set_freq_ki(double ki) {
    freq_ki = ki; freq_pid = new PID(freq_kp, freq_ki, freq_kd);
    log_event("frequency PID constants updated (" + to_str(freq_kp) + "," + to_str(freq_ki) + "," + to_str(freq_kd) + ")");
}
void Gyrotron::set_freq_kd(double kd) {
    freq_kd = kd; freq_pid = new PID(freq_kp, freq_ki, freq_kd);
    log_event("frequency PID constants updated (" + to_str(freq_kp) + "," + to_str(freq_ki) + "," + to_str(freq_kd) + ")");
}

int Gyrotron::toggle_cath_output(bool turn_on)
{
    std::string cmd = "98," + std::string(turn_on ? "1," : "0,");
    std::string resp = cath.smart_io(cmd,"98,$",true);
    if(err(resp))
    {
        log_event("failed to " + std::string(turn_on ? "enable" : "disable") + " cathode output");
        log_event("moving cathode to reconnect loop");
        return -1;
    }
    log_event(std::string(turn_on ? "enabled" : "disabled") + " cathode output");
    cath_hv_on = turn_on;
    return 0;
}

int Gyrotron::toggle_gtc_output(bool turn_on)
{
    std::string cmd = "PWR " + std::string(turn_on ? "ON" : "OFF");
    std::string resp = gtc.m_smart_write(cmd,"PWR",std::string(turn_on ? "ON" : "OFF"));
    if(err(resp))
    {
        log_event("failed to " + std::string(turn_on ? "enable" : "disable") + " GTC output");
        log_event("moving GTC to reconnect loop");
        return -1;
    }
    log_event(std::string(turn_on ? "enabled" : "disabled") + " GTC output");
    gtc_hv_on = turn_on;
    return 0;
}

void Gyrotron::toggle_beam_pid() { toggle_beam_pid(!beam_pid_on); }
void Gyrotron::toggle_power_pid() { toggle_power_pid(!power_pid_on); } 
void Gyrotron::toggle_freq_pid() { toggle_freq_pid(!freq_pid_on); } 

void Gyrotron::toggle_beam_pid(bool turn_on)
{
    if(turn_on)
    {
        freq_pid_on = power_pid_on = false;
        beam_curr_sp.store(beam_curr);
        beam_curr_sp_data.push_back(beam_curr_sp);
        beam_curr_time_data.push_back(runtime());
        beam_pid = new PID(beam_kp, beam_ki, beam_kd); // re-initialize PID controller
        reset_pid_time = true; // FIX THIS
    } 
    beam_pid_on = turn_on;
    log_event("automatic beam control " + std::string(turn_on ? "enabled" : "disabled"));
}

void Gyrotron::toggle_power_pid(bool turn_on)
{
    if(turn_on)
    {
        freq_pid_on = beam_pid_on = false;
        power_sp.store(power);
        power_sp_data.push_back(power_sp);
        power_time_data.push_back(runtime());
        power_pid = new PID(power_kp, power_ki, power_kd); // re-initialize PID controller
        reset_pid_time = true;
    }
    power_pid_on = turn_on;
    log_event("automatic power control " + std::string(turn_on ? "enabled" : "disabled"));
}

void Gyrotron::toggle_freq_pid(bool turn_on)
{
    if(turn_on)
    {
        beam_pid_on = power_pid_on = false;
        freq_sp.store(freq);
        beam_volt_ref.store(beam_volt);
        freq_pid = new PID(freq_kp, freq_ki, freq_kd); // re-initialize PID controller
        reset_pid_time = true;
    }
    freq_pid_on = turn_on;
    log_event("automatic frequency control " + std::string(turn_on ? "enabled" : "disabled"));
}

void Gyrotron::turn_off_pid()
{
    freq_pid_on = false;
    power_pid_on = false;
    beam_pid_on = false;
    log_event("automatic control disabled");
}

int Gyrotron::enter_standby()
{
    beam_pid_on = power_pid_on = freq_pid_on = false;
    ramping_up = ramping_down = false;
    int stat = toggle_cath_output(false);
    if(stat < 0) return -1;
    stat = toggle_gtc_output(false);
    if(stat < 0) return -2;
    return 0;
}

void Gyrotron::set_beam_curr(double c) {
    beam_curr_sp = c;
    beam_curr_sp_data.push_back(beam_curr_sp);
    beam_curr_time_data.push_back(runtime());
    beam_pid = new PID(beam_kp, beam_ki, beam_kd); // re-initialize PID controller
    reset_pid_time = true;
    log_event("applied new beam PID setpoint at " + to_str(c) + "A");
}
void Gyrotron::set_freq(double f) {
    freq_sp = f;
    freq_pid = new PID(freq_kp, freq_ki, freq_kd); // re-initialize PID controller
    reset_pid_time = true;
    log_event("applied new freq PID setpoint at " + to_str(f) + "GHz");
}
void Gyrotron::set_power(double p) {
    power_sp = p;
    power_sp_data.push_back(power_sp);
    power_time_data.push_back(runtime());
    power_pid = new PID(power_kp, power_ki, power_kd); // re-initialize PID controller
    reset_pid_time = true;
    log_event("applied new power PID setpoint at " + to_str(p) + "W");
}

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
        if(prepare_all() < 0) { return -2; } ramping_up = true; current_state++; log_event("begin ramp up"); break;
    case 1: 
        if(ramping_down && !paused) { paused = true; log_event("paused ramp"); }
        else if(ramping_down && paused) { ramping_down = false; ramping_up = true; paused = false; log_event("begin ramp up"); }
        else if(ramping_up && paused) { paused = false; log_event("resume ramp up");}
        break;
    case 2: if(toggle_mw(true) < 0) { return -2; } current_state++; log_event("MW on"); break;
    }
    return 0;
}

int Gyrotron::decrement_state()
{
    if(current_state == 0)
        return -1;
    
    switch(current_state)
    {
    case 1: if(ramping_up && !paused) { paused = true; log_event("paused ramp"); }
        else if(ramping_up && paused) { ramping_up = false; ramping_down = true; paused = false; log_event("begin ramp down"); }
        else if(ramping_down && paused) { paused = false; log_event("resume ramp down"); }
        break;
    case 2: ramping_down = true; current_state--; log_event("begin ramp down"); break;
    case 3: if(toggle_mw(false) < 0) { return -2; } current_state--; log_event("MW off"); break;
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
    if(set_beam_volt(BEAM_VOLT_LIMIT) < 0 || set_gtc_curr_limit(GTC_CURR_LIMIT) < 0)
        return -1;
    return 0;
}

bool Gyrotron::all_clear()
{
    if(cath.is_enabled() && gtc.is_enabled() && spc.is_enabled() && rsi.is_enabled() && fms.is_enabled() &&
       cath.is_connected() && gtc.is_connected() && spc.is_connected() && rsi.is_connected() && fms.is_connected() &&
       num_sys_errors() == 0 && cath.num_errors() == 0)
    { return true; }

    return false;
}

int Gyrotron::get_temp_status() // 0 = safe temp, -1 = warning, -2 = fatal
{
    if(main_chill_temp > FATAL_TEMP || cav_chill_temp > FATAL_TEMP || collector_temp > FATAL_TEMP || cav_temp > FATAL_TEMP || body_temp > FATAL_TEMP)
        return -2;
    else if(main_chill_temp > WARN_TEMP || cav_chill_temp > WARN_TEMP || collector_temp > WARN_TEMP || cav_temp > WARN_TEMP || body_temp > WARN_TEMP)
        return -1;
    return 0;
}

int Gyrotron::get_flow_status() // 0 = safe flow, -1 = warning, -2 = fatal
{
    if(gun_air_flow < FATAL_AIR_FLOW || main_chill_flow < FATAL_WATER_FLOW || cav_chill_flow < FATAL_WATER_FLOW || collector_flow < FATAL_WATER_FLOW)
        return -2;
    else if(gun_air_flow < WARN_AIR_FLOW || main_chill_flow < WARN_WATER_FLOW || cav_chill_flow < WARN_WATER_FLOW || collector_flow < WARN_WATER_FLOW)
        return -1;
    return 0;
}

int Gyrotron::get_fault_status() // 0 = no faults, -1 = warnings, -2 = errors
{
    if(get_num_errors() > 0)
        return -2;
    else if(get_num_warnings() > 0)
        return -1;
    return 0;
}

int Gyrotron::get_press_status() // 0 = safe pressure, -1 = relax, -2 = fatal
{
    if(pressure > fatal_press)
        return -2;
    else if(pressure > press_bound2)
        return -1;
    return 0;
}

// add logic here to be done following the device's reconnect
void Gyrotron::cath_recon() {}
void Gyrotron::gtc_recon() {}
void Gyrotron::spc_recon() {}
void Gyrotron::fms_recon() {}
void Gyrotron::rsi_recon() {}

double Gyrotron::get_temp(int num) // 1=main chiller, 2=cavity chiller, 3=collector, 4=cavity, 5=body
{
    switch(num)
    {
    case 1: return main_chill_temp;
    case 2: return cav_chill_temp;
    case 3: return collector_temp;
    case 4: return cav_temp;
    case 5: return body_temp;
    default: return -1;
    }
}
double Gyrotron::get_flow(int num) // 1=main chiller, 2=cavity chiller, 3=collector, 4=gun air
{
    switch(num)
    {
    case 1: return main_chill_flow;
    case 2: return cav_chill_flow;
    case 3: return collector_flow;
    case 4: return gun_air_flow;
    default: return -1;
    }
}
