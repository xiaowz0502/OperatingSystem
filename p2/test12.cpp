#include <iostream>
#include "thread.h"
#include "mutex.h"
#include "cv.h"

using std::cout;
using std::endl;

void funcB(void *argv)
{
    // thread::yield();
    cout << "done: B" << endl;
}

void func(void *argv)
{
    thread tB(reinterpret_cast<thread_startfunc_t>(funcB), const_cast<void *>(static_cast<const void *>("B")));
    assert_interrupts_enabled();
    tB.join();
    cout << "done: A" << endl;
}

void run(void *arg)
{
    assert_interrupts_enabled();
    thread tA(reinterpret_cast<thread_startfunc_t>(func), const_cast<void *>(static_cast<const void *>("A")));
    tA.join();
}

int main()
{
    cpu::boot(1, reinterpret_cast<thread_startfunc_t>(run), nullptr, true, false, 10);
}