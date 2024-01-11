#include "common.h"
#include "thread.h"
#include "thread_impl.h"
#include <memory>
#include <iostream>

using std::cout;
using std::endl;


thread::thread(thread_startfunc_t func, void *argv)
{
    cpu::interrupt_disable();
    impl_ptr = new impl;
    create_context(impl_ptr);
    makecontext(impl_ptr->ucontext_ptr.get(), reinterpret_cast<void (*)()>(wrapper), 2, func, argv);
    ready_queue.push_back(impl_ptr);
    cpu::interrupt_enable();
}

void thread::join()
{
    cpu::interrupt_disable();
    if (alive_list[this->impl_ptr->id] == true)
    {
        this->impl_ptr->waiting_queue.push_back(running_ptr);
        if (!ready_queue.empty())
        {
            thread::impl *temp = running_ptr;
            running_ptr = ready_queue.front();
            ready_queue.pop_front();
            swapcontext(temp->ucontext_ptr.get(), running_ptr->ucontext_ptr.get());
        }
        else
        {
            cpu::interrupt_enable_suspend();
        }
    }
    cpu::interrupt_enable();
}

void thread::yield()
{
    cpu::interrupt_disable();
    if (!ready_queue.empty())
    {
        ready_queue.push_back(running_ptr);
        thread::impl *temp = running_ptr;
        running_ptr = ready_queue.front();
        ready_queue.pop_front();
        swapcontext(temp->ucontext_ptr.get(), running_ptr->ucontext_ptr.get());
    }
    cpu::interrupt_enable();
}

thread::~thread() {}