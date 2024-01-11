
#include "mutex.h"
#include "mutex_impl.h"
#include "common.h"
#include "thread_impl.h"

mutex::mutex()
{
    cpu::interrupt_disable();
    impl_ptr = new impl;
    impl_ptr->status = 'f';
    impl_ptr->curr_hold = -1;
    cpu::interrupt_enable();
}
mutex::~mutex()
{
    cpu::interrupt_disable();
    impl_ptr->waiting_queue.clear();
    delete impl_ptr;
    cpu::interrupt_enable();
}

void mutex::lock()
{
    cpu::interrupt_disable();
    lock_handler(this->impl_ptr);
    cpu::interrupt_enable();
}


void mutex::unlock()
{
    cpu::interrupt_disable();
    unlock_handler(this->impl_ptr);
    cpu::interrupt_enable();
}
