#include <iostream>
#include "thread.h"
#include "mutex.h"
#include "cv.h"

using std::cout;
using std::endl;

void funcB(void *argv)
{
    cout << "start: B yield" << endl;
    thread::yield();
    cout << "back and done: B" << endl;
}

void func(void *argv)
{
    cout << "start: A" << endl;
    thread tB(reinterpret_cast<thread_startfunc_t>(funcB), const_cast<void *>(static_cast<const void *>("B")));
    assert_interrupts_enabled();
    tB.join();
    cout << "done: A" << endl;
}

void funcC(void *argv)
{
    assert_interrupts_enabled();
    cout << "start: C" << endl;
    thread::yield();
    cout << "done: C" << endl;
}
void run(void *arg)
{
    assert_interrupts_enabled();
    thread tA(reinterpret_cast<thread_startfunc_t>(func), const_cast<void *>(static_cast<const void *>("A")));

    thread tC(reinterpret_cast<thread_startfunc_t>(funcC), const_cast<void *>(static_cast<const void *>("C")));
    tA.join();
    cout << "A joined " << endl;
    tC.join();
     cout <<"C joined " << endl;
}

int main()
{
    cpu::boot(1, reinterpret_cast<thread_startfunc_t>(run), nullptr, true, false, 10);
}