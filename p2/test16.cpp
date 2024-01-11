#include <iostream>
#include "thread.h"
#include "mutex.h"
#include "cv.h"

mutex x, z;
cv y;
using std::cout;
using std::endl;

void funcA(void *a)
{
    try
    {
        x.unlock();
    }
    catch (std::runtime_error &e)
    {
        cout << "catch error" << endl;
    }
}

void funcB(void *a)
{
    try
    {
        y.wait(z);
        z.unlock();
    }
    catch (std::runtime_error &f)
    {
        cout << "catch error" << endl;
    }
}

void run(void *arg)
{
    thread tA(reinterpret_cast<thread_startfunc_t>(funcA), const_cast<void *>(static_cast<const void *>("A")));
    thread tB(reinterpret_cast<thread_startfunc_t>(funcB), const_cast<void *>(static_cast<const void *>("B")));
}

int main()
{
    cpu::boot(1, reinterpret_cast<thread_startfunc_t>(run), nullptr, true, false, 10);
}
