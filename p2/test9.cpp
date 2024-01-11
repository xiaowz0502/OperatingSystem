#include <iostream>
#include "thread.h"
#include "mutex.h"
#include "cv.h"
// BNQ
mutex x, y, c;

void child1()
{
 for (size_t i = 0; i < 1000; i++)
 {
  c.lock();
  std::cout << "child1: " << i << std::endl;
  c.unlock();

  x.lock();
  y.lock();
  y.unlock();
  x.unlock();
 }
}

void child2()
{
 for (int i = 0; i < 1000; i++)
 {

  c.lock();
  std::cout << "child2: " << i << std::endl;
  c.unlock();

  y.lock();
  x.lock();
  x.unlock();
  y.unlock();
 }
}

void run()
{
 thread t1(reinterpret_cast<thread_startfunc_t>(child1), reinterpret_cast<void *>(1));
 thread t2(reinterpret_cast<thread_startfunc_t>(child2), reinterpret_cast<void *>(2));

 t1.join();
 t2.join();
}

int main()
{
 cpu::boot(1, reinterpret_cast<thread_startfunc_t>(run), nullptr, true, false, 0);
}