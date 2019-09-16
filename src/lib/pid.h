#include <atomic>

class PID
{
    public:
        PID(double p, double i, double d) : Kp(p), Kd(d), Ki(i) {}
        PID(double p, double i, double d, double mx, double mn) :
            Kp(p), Kd(d), Ki(i), max(mx), min(mn) { max_set = min_set = true; }
        double calculate(double setpoint, double value, double dt)
	{
        double error = setpoint - value; // calculate error
	    double Pout = Kp * error; // proportional term
	    integral += error * dt; // adjust integral multiplier
	    double Iout = Ki * integral; // integral term
	    double derivative = (error - preerror) / dt; // calculate derivative multiplier
	    double Dout = Kd * derivative; // derivative term
	    double output = Pout + Iout + Dout; // calculate total output

	    if(max_set && output > max)
		output = max;
	    else if(min_set && output < min)
		output = min;

	    preerror = error; // store error for next calculation
	    return output;
	}
        void set_max(double new_max) { max = new_max; max_set = true; }
        void set_min(double new_min) { min = new_min; min_set = true; }

    private:
        double Kp{0},Kd{0},Ki{0}; // proportional, derivative, and integral gain constants
        double preerror{0}; // stores previous error
        double integral{0}; // stores integral multiplier
        std::atomic<double> max, min; // bounds for value adjustment
        std::atomic<bool> max_set{false}, min_set{false}; // track whether max or min is in effect
};
// might add functions to change K constants
// not sure yet how that would affect peformance vs. initializing new controller
