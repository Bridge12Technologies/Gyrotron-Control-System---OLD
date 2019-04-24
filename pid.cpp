#ifndef _PID_SOURCE_
#define _PID_SOURCE_

#include <iostream>
#include <cmath>
#include "pid.h"

using namespace std;

class PIDImpl
{
    public:
        PIDImpl(double Kp, double Ki, double Kd);
        ~PIDImpl();
        double calculate(double setpoint, double pv, double dt);

    private:
        double Kp;
        double Kd;
        double Ki;
        double preerror;
        double integral;
};

PID::PID(double Kp, double Ki, double Kd)
{
    pimpl = new PIDImpl(Kp,Ki,Kd);
}
double PID::calculate( double setpoint, double pv, double dt )
{
    return pimpl->calculate(setpoint,pv,dt);
}
PID::~PID() { delete pimpl; }

PIDImpl::PIDImpl(double Kp, double Ki, double Kd) :
    Kp(Kp), Kd(Kd), Ki(Ki), preerror(0), integral(0) {}

double PIDImpl::calculate(double setpoint, double pv, double dt)
{
    // Calculate error
    double error = setpoint - pv;

    cout << "PID: error=" << error;

    // Proportional term
    double Pout = Kp * error;

    cout << ", Pout=" << Pout;

    // Integral term
    integral += error * dt;
    double Iout = Ki * integral;

    cout << ", Iout=" << Iout;

    // Derivative term
    double derivative = (error - preerror) / dt;
    double Dout = Kd * derivative;

    cout << ", Dout=";

    // Calculate total output
    double output = Pout + Iout + Dout;

    cout << ", total=" << output << endl;
/*
    // Restrict to max/min
    if( output > max )
        output = max;
    else if( output < min )
        output = min;
*/
    // Save error to previous error
    preerror = error;

    return output;
}

PIDImpl::~PIDImpl() {}

#endif
