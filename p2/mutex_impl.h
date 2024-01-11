#include "mutex.h"
#include <deque>
#include "thread.h"

class mutex::impl
{
public:
    std::deque<thread::impl *> waiting_queue;
    char status;
    int curr_hold;
};
