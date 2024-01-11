#include <iostream>
#include "cpu.h"
#include "thread.h"

mutex lock;

void child()
{
    lock.lock();
    std::cout << "done" << std::endl;
    lock.unlock();
}

void run()
{
    thread t3 = thread(reinterpret_cast<thread_startfunc_t>(child), reinterpret_cast<void *>(3));
}

int main()
{
    cpu::boot(1, reinterpret_cast<thread_startfunc_t>(run), nullptr, false, false, 0);
    return 0;
}
