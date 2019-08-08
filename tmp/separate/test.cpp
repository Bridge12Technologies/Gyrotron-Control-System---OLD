// compile with -pthread and -std=c++11
// this is a proof-of-concept for passing member functions between classes

#include <iostream>
#include <functional>
#include <thread>

using namespace std::placeholders;

class B;

class A
{
private:
    int var_a{5};
    B *b;

public:
    A();
    void update_var() { var_a += 5; }
    int get_var() { return var_a; } 
};

class B
{
public:
    void exec_func(std::function<void()> func) { std::thread t(func); t.join(); }
};

A::A() { auto fp = std::bind(&A::update_var, this); b->exec_func(fp); }

int main()
{
    A a;
    std::cout << a.get_var() << "\n";
    return 0;
}