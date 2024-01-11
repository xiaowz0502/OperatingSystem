#include "thread.h"
#include <deque>

//global variables
extern thread::impl *running_ptr;
extern std::deque<thread::impl *> ready_queue;
extern thread::impl *prev_delete;
extern int num_thread;
extern std::deque<bool> alive_list;

extern void wrapper(thread_startfunc_t func, void *argv);
extern void handle_end();
extern void create_context(thread::impl* ptr);
extern void lock_handler(mutex::impl *locker);
extern void unlock_handler(mutex::impl *locker);