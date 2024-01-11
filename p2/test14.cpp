#include <iostream>
#include "thread.h"
#include "mutex.h"
#include "cv.h"

mutex x, y;
using std::cout;
using std::endl;

void funcA(void *a)
{
    try
    {
        cout << "A acquire lock x" << endl;
        x.lock();
        cout << "A release lock Y" << endl;
        y.unlock();
    }
    catch (std::runtime_error &e)
    {
        cout << "catch error from B"<<endl;
    }
}

void funcB(void *a)
{
    try
    {
        cout << "B acquire lock Y" << endl;
        y.lock();

        cout << "B acquire lock X" << endl;
        x.unlock();
    }
    catch (std::runtime_error &f)
    {
        cout << "catch error from B"<<endl;
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
