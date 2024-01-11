#include <iostream>
#include "cpu.h"
#include "thread.h"
#include "cv.h"
#include "mutex.h"
#include <deque>
#include <memory>

mutex m1;
cv cv1;

std::deque<std::shared_ptr<thread>> vec;

void func1()
{
    m1.lock();
    cv1.wait(m1);
    for (size_t i = 0; i < 1000; i++) {
        std::cout << "func1";
    }
    std::cout << std::endl;
    m1.unlock();
}

void func(void* t)
{   
    vec[0]->join();
    std::cout << reinterpret_cast<intptr_t>(t) << " :done after join" << std::endl;
}

void func_broadcast()
{
    cv1.broadcast();
    std::cout << "11: broadcast done" << std::endl;
}

void run()
{
    std::shared_ptr<thread> t1 = std::make_shared<thread>(reinterpret_cast<thread_startfunc_t>(func1), reinterpret_cast<void *>(1));
    vec.push_back(t1);
    thread t2 = thread(reinterpret_cast<thread_startfunc_t>(func), reinterpret_cast<void *>(2));
    thread t3 = thread(reinterpret_cast<thread_startfunc_t>(func), reinterpret_cast<void *>(3));
    thread t4 = thread(reinterpret_cast<thread_startfunc_t>(func), reinterpret_cast<void *>(4));
    thread t5 = thread(reinterpret_cast<thread_startfunc_t>(func), reinterpret_cast<void *>(5));
    thread t6 = thread(reinterpret_cast<thread_startfunc_t>(func), reinterpret_cast<void *>(6));
    thread t7 = thread(reinterpret_cast<thread_startfunc_t>(func), reinterpret_cast<void *>(7));
    thread t8 = thread(reinterpret_cast<thread_startfunc_t>(func), reinterpret_cast<void *>(8));
    thread t9 = thread(reinterpret_cast<thread_startfunc_t>(func), reinterpret_cast<void *>(9));
    thread t10 = thread(reinterpret_cast<thread_startfunc_t>(func), reinterpret_cast<void *>(10));
    thread t11 = thread(reinterpret_cast<thread_startfunc_t>(func_broadcast), reinterpret_cast<void *>(2));
}

int main()
{
    cpu::boot(1, reinterpret_cast<thread_startfunc_t>(run), nullptr, false, false, 0);
    return 0;
}