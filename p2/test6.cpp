#include <iostream>
#include <cstdlib>
#include "thread.h"
//ABEKO
using std::cout;
using std::endl;

int g = 0;

mutex mutex1;
cv cv1;

void loop(void *a)
{
    auto id = static_cast<char *>(a);
    int i;

    mutex1.lock();
    cout << "loop called with id " << id << endl;

    for (i=0; i<5; i++, g++) {
	cout << id << ":\t" << i << "\t" << g << endl;
        mutex1.unlock();
	thread::yield();
        mutex1.lock();
    }
    cout << id << ":\t" << i << "\t" << g << endl;
    // mutex1.unlock();
}

void parent(void *a)
{
    auto arg = reinterpret_cast<intptr_t>(a);

    mutex1.lock();
    cout << "parent called with arg " << arg << endl;
    mutex1.unlock();

    thread t1 (reinterpret_cast<thread_startfunc_t>(loop), const_cast<void *>(static_cast<const void *>("child thread")));

    loop(const_cast<void *>(static_cast<const void *>("parent thread")));
}

int main()
{
    cpu::boot(1, reinterpret_cast<thread_startfunc_t>(parent), reinterpret_cast<void *>(100), true, true, 0);
}