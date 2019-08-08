#include "spine.h"

class test_class : public Spine
{
public:
    test_class() : Spine(false,false)
    {
        //add_device(&dev,std::bind(&test_class::increment, this),
        //                    std::bind(&test_class::rfunc, this),"test","test");
        //add_pre_func(std::bind(&test_class::prefunc, this));
        //add_post_func(std::bind(&test_class::print_val, this));
        add_pre_func([=](){increment(5);});
        add_post_func([=](){print_val();});
    }
    void increment(int num) { test_value += num; } // using this for query function
    void print_val() { std::cout << "(post) final value: " << test_value << "\n"; }
    void prefunc() { std::cout << "(pre) starting value: " << test_value << "\nwaiting...\n"; }
    void rfunc() { std::cout << "this is the reconnect function\n"; }
    void single_test() { std::cout << "this is a single execution function\n"; }

private:
    std::atomic<int> test_value{0};
    device dev{"ARD"};
};

// expecting an initial print statement, followed by delay
// followed by printout of some number > 0
// logs should be created and mostly empty
// reconnect function should not have to execute

int main()
{
    test_class tc;

    tc.init();
    usleep(3e6);
    tc.stop();

    return 0;
}