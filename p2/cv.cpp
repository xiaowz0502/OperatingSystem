#include "cv.h"
#include "mutex.h"
#include "cv_impl.h"
#include "common.h"
#include "thread_impl.h"
#include "mutex_impl.h"

cv::cv()
{
    cpu::interrupt_disable();
    impl_ptr = new impl;
    impl_ptr->target = nullptr;
    cpu::interrupt_enable();
}

cv::~cv()
{
    cpu::interrupt_disable();
    impl_ptr->sleeping_queue.clear();
    delete impl_ptr;
    cpu::interrupt_enable();
}

void cv::wait(mutex &locker)
{
    cpu::interrupt_disable();
    
    unlock_handler(locker.impl_ptr);

    this->impl_ptr->sleeping_queue.push_back(running_ptr);
    this->impl_ptr->target = &locker;

    if (!ready_queue.empty()) 
    {
        thread::impl *next = ready_queue.front();
        thread::impl *curr = running_ptr;
        running_ptr = next;
        ready_queue.pop_front();
        swapcontext(curr->ucontext_ptr.get(), running_ptr->ucontext_ptr.get());
    } 
    else 
    {
        cpu::interrupt_enable_suspend();
    }

    lock_handler(locker.impl_ptr);

    cpu::interrupt_enable();
}

void wake_up_helper(cv::impl *cv_ptr){
    thread::impl *wake_up = cv_ptr->sleeping_queue.front();
    cv_ptr->sleeping_queue.pop_front();
    ready_queue.push_back(wake_up);
}

void cv::signal()
{
    cpu::interrupt_disable();
    if (!this->impl_ptr->sleeping_queue.empty() && this->impl_ptr->target != nullptr)
    {
        wake_up_helper(this->impl_ptr);
    }
    cpu::interrupt_enable();
}

void cv::broadcast()
{
    cpu::interrupt_disable();
    if (!this->impl_ptr->sleeping_queue.empty() && this->impl_ptr->target != nullptr)
    {
        size_t size = this->impl_ptr->sleeping_queue.size();
        for (size_t i = 0; i < size; i++)
        {
            wake_up_helper(this->impl_ptr);
        }
    }
    cpu::interrupt_enable();
}