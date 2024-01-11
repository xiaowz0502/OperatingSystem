#include "thread.h"
#include <deque>
#include "cv.h"
#include "mutex.h"

class cv::impl
{
public:
    std::deque<thread::impl *> sleeping_queue;
    mutex *target;
};
