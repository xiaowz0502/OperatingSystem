#include <iostream>
#include "thread.h"
#include "mutex.h"
#include "cv.h"

mutex m;
cv cv1;

void func(void *a)
{
    char id = *(static_cast<char *>(a));
    m.lock();
    std::cout << id << " locked" << std::endl;
    cv1.wait(m);
    std::cout << id <<" wait end" << std::endl;
    m.unlock();
    std::cout << id <<" unlocked" << std::endl;
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
    std::cout << "broadcasting!!" << std::endl;
    cv1.broadcast();
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
    cpu::boot(1, reinterpret_cast<thread_startfunc_t>(run), nullptr, true, false, 10);
}