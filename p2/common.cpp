#include "common.h"
#include "thread_impl.h"
#include "mutex_impl.h"

thread::impl *running_ptr = nullptr;
std::deque<thread::impl *> ready_queue;
thread::impl *prev_delete = nullptr;
int num_thread = 0;
std::deque<bool> alive_list;

void wrapper(thread_startfunc_t func, void *argv)
{
    cpu::interrupt_enable();
    func(argv);
    cpu::interrupt_disable();
    handle_end();
}

void handle_end()
{
    if (!running_ptr->waiting_queue.empty())
    {
        size_t size = running_ptr->waiting_queue.size();
        for (size_t i = 0; i < size; i++)
        {
            ready_queue.push_back(running_ptr->waiting_queue.front());
            running_ptr->waiting_queue.pop_front();
        }
    }

    alive_list[running_ptr->id] = false;

    if (prev_delete == nullptr)                                 //ensure O(1) memory leak
    {
        prev_delete = running_ptr;
    }
    else
    {
        delete[] (char *)prev_delete->stack_pointer;
        delete prev_delete;
        prev_delete = running_ptr;
    }

    if (!ready_queue.empty())
    {
        running_ptr = ready_queue.front();
        ready_queue.pop_front();
        setcontext(running_ptr->ucontext_ptr.get());
    }
    else
    {
        cpu::interrupt_enable_suspend();
    }
}

void create_context(thread::impl* ptr) {
    try
    {
        char *stack = new char[STACK_SIZE];
        ptr->stack_pointer = stack;
        ptr->ucontext_ptr->uc_stack.ss_sp = stack;
        ptr->ucontext_ptr->uc_stack.ss_size = STACK_SIZE;
        ptr->ucontext_ptr->uc_stack.ss_flags = 0;
        ptr->ucontext_ptr->uc_link = nullptr;
        ptr->id = num_thread;
        alive_list.push_back(true);
        num_thread++;
    }
    catch(std::bad_alloc &e)
    {
        cpu::interrupt_enable();                                //avoid double disabling
        throw;
    }
}

void lock_handler(mutex::impl *locker){
    if (locker->status == 'f')
    {
        locker->curr_hold = running_ptr->id;
        locker->status = 'b';
    }
    else
    {
        locker->waiting_queue.push_back(running_ptr);
        
        if (!ready_queue.empty()) 
        {
            thread::impl *next_thread = ready_queue.front();
            thread::impl *temp = running_ptr;
            running_ptr = next_thread;
            ready_queue.pop_front();
            swapcontext(temp->ucontext_ptr.get(), running_ptr->ucontext_ptr.get());
        } 
        else 
        {
            cpu::interrupt_enable_suspend();
        }
    }
}


void unlock_handler(mutex::impl *locker){
    if (locker->curr_hold != running_ptr->id)
    {
        cpu::interrupt_enable();
        throw std::runtime_error("");
    }
    locker->status = 'f';
    if (!locker->waiting_queue.empty())
    {
        thread::impl *ready = locker->waiting_queue.front();
        locker->waiting_queue.pop_front();
        ready_queue.push_back(ready);
        locker->curr_hold = ready->id;
        locker->status = 'b';
    }
    else
    {
        locker->curr_hold = -1;
    }
}