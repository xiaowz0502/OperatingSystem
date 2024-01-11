#include <iostream>
#include "thread.h"
#include "mutex.h"
#include "cv.h"

mutex m;

void func(void *a) {
   char id = *(static_cast<char*>(a));
   std::cout << id << " pre-lock" << std::endl;
   m.lock();
   std::cout << id << " critical" << std::endl;
   if (id == 'A') {
      thread::yield();
      thread tC(reinterpret_cast<thread_startfunc_t>(func), const_cast<void*>(static_cast<const void*>("C")));
   }
   m.unlock();
   std::cout << id << " done" << std::endl;
}

void run(void *arg) {
   thread tA(reinterpret_cast<thread_startfunc_t>(func), const_cast<void*>(static_cast<const void*>("A")));
   thread tB(reinterpret_cast<thread_startfunc_t>(func), const_cast<void*>(static_cast<const void*>("B")));
}

int main()
{
    cpu::boot(1, reinterpret_cast<thread_startfunc_t>(run), nullptr, false, false, 0);
}
