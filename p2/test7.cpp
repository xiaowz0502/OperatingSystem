#include <iostream>
#include "thread.h"
#include "mutex.h"
#include "cv.h"

mutex m;

void func(void *a) {
}

void run(void *arg) {
   thread tA(reinterpret_cast<thread_startfunc_t>(func), const_cast<void*>(static_cast<const void*>("A")));
   thread::yield();
   tA.join();
}

int main()
{
    cpu::boot(1, reinterpret_cast<thread_startfunc_t>(run), nullptr, false, false, 0);
}
