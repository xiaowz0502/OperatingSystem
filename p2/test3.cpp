#include <iostream>
#include "thread.h"
#include "mutex.h"
#include "cv.h"

mutex m;
cv cv1;

void func(void *a)
{
    char id = *(static_cast<char *>(a));
    std::cout << id << " pre-lock" << std::endl;
    m.lock();
    cv1.wait(m);
    m.unlock();
    std::cout << id << " done" << std::endl;
}

void funcReleaseSignal(void *a)
{
    for (int i = 0; i < 4; i++)
    {
        std::cout << "relseasing!!" << std::endl;
        cv1.signal();
    }
}

void funcReleaseBroadcasting(void *a)
{
    m.lock();
    cv1.broadcast();
    m.unlock();
}

void run(void *arg)
{
    cv1.signal();
    cv1.broadcast();
    thread tA(reinterpret_cast<thread_startfunc_t>(func), const_cast<void *>(static_cast<const void *>("A")));
    thread tB(reinterpret_cast<thread_startfunc_t>(func), const_cast<void *>(static_cast<const void *>("B")));
    thread tC(reinterpret_cast<thread_startfunc_t>(func), const_cast<void *>(static_cast<const void *>("C")));
    thread tD(reinterpret_cast<thread_startfunc_t>(func), const_cast<void *>(static_cast<const void *>("D")));
    thread tE(reinterpret_cast<thread_startfunc_t>(funcReleaseSignal), const_cast<void *>(static_cast<const void *>("E")));
    thread tF(reinterpret_cast<thread_startfunc_t>(func), const_cast<void *>(static_cast<const void *>("F")));
    thread tG(reinterpret_cast<thread_startfunc_t>(func), const_cast<void *>(static_cast<const void *>("G")));
    thread tH(reinterpret_cast<thread_startfunc_t>(func), const_cast<void *>(static_cast<const void *>("H")));
    thread tI(reinterpret_cast<thread_startfunc_t>(func), const_cast<void *>(static_cast<const void *>("I")));
    thread tK(reinterpret_cast<thread_startfunc_t>(funcReleaseBroadcasting), const_cast<void *>(static_cast<const void *>("K")));
}

int main()
{
    cpu::boot(1, reinterpret_cast<thread_startfunc_t>(run), nullptr, false, false, 0);
}