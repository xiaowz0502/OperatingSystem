#include <iostream>
#include "thread.h"
#include "mutex.h"
#include "cv.h"

mutex m;
cv cv1;

void func(void *a)
{
    // char id = *(static_cast<char *>(a));
    // std::cout << id << " pre-lock" << std::endl;
    // m.lock();
    // cv1.wait(m);
    // m.unlock();
    // std::cout << id << " done" << std::endl;
    m.lock();
    int i = 0;
    while (i != 100)
    {
        cv1.wait(m);
        i++;
    }
    m.unlock();
}

void funcB(void *a)
{
    m.lock();
    int i = 0;
    while (i != 1)
    {
        cv1.wait(m);
        i++;
    }
    m.unlock();
}

void funcReleaseSignal(void *a)
{
    for (int i = 0; i < 100; i++)
    {
        // std::cout << "relseasing!!" << std::endl;
        cv1.signal();
    }
}

// void funcReleaseBroadcasting(void *a)
// {
//     cv1.broadcast();
// }

void run(void *arg)
{
    thread tB(reinterpret_cast<thread_startfunc_t>(func), const_cast<void *>(static_cast<const void *>("B")));
    thread tA(reinterpret_cast<thread_startfunc_t>(funcB), const_cast<void *>(static_cast<const void *>("A")));
    thread tE(reinterpret_cast<thread_startfunc_t>(funcReleaseSignal), const_cast<void *>(static_cast<const void *>("E")));
}

int main()
{
    cpu::boot(1, reinterpret_cast<thread_startfunc_t>(run), nullptr, true, false, 10);
}