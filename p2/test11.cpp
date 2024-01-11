#include <iostream>
#include "thread.h"
#include "mutex.h"
#include "cv.h"

using std::cout;
using std::endl;

#include <iostream>
#include "thread.h"
#include "mutex.h"
#include "cv.h"

using std::cout;
using std::endl;

void func2(void *argv)
{
    thread::yield();
    cout << "done: B" << endl;
}

void func(void *argv)
{
    thread tB(reinterpret_cast<thread_startfunc_t>(func2), const_cast<void *>(static_cast<const void *>("B")));
    thread::yield();
    cout << "done: A" << endl;
}

void run(void *arg)
{
    thread tA(reinterpret_cast<thread_startfunc_t>(func), const_cast<void *>(static_cast<const void *>("A")));
    thread::yield();
    std::cout << "back to main thread" << std::endl;
    thread::yield();
    std::cout << "end thread" << std::endl;
}

int main()
{
    cpu::boot(1, reinterpret_cast<thread_startfunc_t>(run), nullptr, true, false, 10);
}