#include "cpu.h"
#include "ucontext.h"
#include "common.h"
#include "thread.h"
#include "thread_impl.h"
#include <deque>
#include <memory>

using std::deque;
using std::shared_ptr;

typedef void (*interrupt_handler_t)(void);


static void timer_handler()
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

void cpu::init(thread_startfunc_t func, void *argv)
{
    cpu::interrupt_vector_table[cpu::TIMER] = timer_handler;
    shared_ptr<ucontext_t> temp = std::make_shared<ucontext_t>();
    thread::impl* init_ptr = new thread::impl;
    create_context(init_ptr);
    makecontext(init_ptr->ucontext_ptr.get(), reinterpret_cast<void (*)()>(wrapper), 2, func, argv);
    running_ptr = init_ptr;
    swapcontext(temp.get(), init_ptr->ucontext_ptr.get());
}