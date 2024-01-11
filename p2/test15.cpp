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
        cout << "catch the error" << endl;
    }
}

void run(void *arg)
{
    thread tA(reinterpret_cast<thread_startfunc_t>(funcA), const_cast<void *>(static_cast<const void *>("A")));
}

int main()
{
    cpu::boot(1, reinterpret_cast<thread_startfunc_t>(run), nullptr, true, false, 10);
}
