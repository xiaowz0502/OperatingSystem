#include "thread.h"
#include "ucontext.h"
#include <deque>
#include <memory>

class thread::impl
{
public:
    std::shared_ptr<ucontext_t> ucontext_ptr;
    std::deque<thread::impl *> waiting_queue;
    char *stack_pointer;
    int id;
    impl()
    {
        ucontext_ptr = std::make_shared<ucontext_t>();
    }
};
