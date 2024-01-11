#include <iostream>
#include "thread.h"
#include "mutex.h"
#include "cv.h"

mutex m;
cv cv1;
using std::cout;
using std::endl;

void func(void *a)
{
    auto id = static_cast<char *>(a);
    std::cout << id;
    std::cout << " pre-lock" << std::endl;
    m.lock();
    cv1.wait(m);
    m.unlock();
    std::cout << id << " done" << std::endl;
}

void funcReleaseSignal(void *a)
{
    // auto id = static_cast<char *>(a);
    // std::cout << id;
    // for (int i = 0; i < 4; i++)
    // {
    std::cout << "relseasing!!" << std::endl;
    cv1.signal();
    // }
}

void funcReleaseBroadcasting(void *a)
{
    cout<<"boardcasting"<<endl;
    cv1.broadcast();
}

void run(void *arg)
{
    thread tA(reinterpret_cast<thread_startfunc_t>(func), const_cast<void *>(static_cast<const void *>("A")));
    thread tE(reinterpret_cast<thread_startfunc_t>(funcReleaseSignal), const_cast<void *>(static_cast<const void *>("E")));
    thread tF(reinterpret_cast<thread_startfunc_t>(func), const_cast<void *>(static_cast<const void *>("F")));
    thread tK(reinterpret_cast<thread_startfunc_t>(funcReleaseBroadcasting), const_cast<void *>(static_cast<const void *>("K")));
}

int main()
{
    cpu::boot(1, reinterpret_cast<thread_startfunc_t>(run), nullptr, true, true, 10);
}
