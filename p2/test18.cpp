// submit
// HJK
#include "thread.h"
#include <ostream>
#include <iostream>
#include <string>
#include <fstream>
#include <deque>
#include <vector>
#include <unordered_map>

using std::find;
using std::ifstream;
using std::string;
using std::to_string;

struct track
{
    int tnum;
    int rid;
};

int max_disk_queue;
int num_requesters;
int num_active = 0;
std::unordered_map<int, bool> requester_map;
std::vector<std::deque<int>> disk_data;
std::deque<track> disk_queue;
mutex disklock;
cv requesterCV;
cv servicerCV;

void requester(void *arg)
{
    // if disk_queue does not fulfill the max_disk_queue, add the next request.
    // if there is already a request from this requester, wait
    disklock.lock();
    int rid = reinterpret_cast<uintptr_t>(arg);
    // check if there is no request from this requester
    if (disk_data[rid].size() == 0)
    {
        disklock.unlock();
        return;
    }
    while (disk_data[rid].size() > 0)
    {
        // determine if disk_queue doesn't fulfills the max condition
        while ((int)disk_queue.size() == (num_active > max_disk_queue ? max_disk_queue : num_active) || requester_map[rid])
        {
            requesterCV.wait(disklock);
            // servicerCV.signal();
        }
        if (disk_data[rid].size() == 0)
        {
            servicerCV.signal();
            break;
        }
        // if disk_queue doesn't have this requester's request, add it
        track temp = {disk_data[rid].front(), rid};
        disk_queue.push_back(temp);
        requester_map[rid] = true;
        servicerCV.signal();
    }
    disklock.unlock();
}

void servicer(void *arg)
{
    // if disk_queue fulfills the max_disk_queue, do the next request
    disklock.lock();
    int cur_track = 0;
    track cur_task;
    while (true)
    {
        // end condition
        if (num_active == 0 && disk_queue.size() == 0)
        {
            break;
        }
        while ((int)disk_queue.size() != (num_active > max_disk_queue ? max_disk_queue : num_active))
        {
            // requesterCV.broadcast();
            servicerCV.wait(disklock);
        }
        // find the closest track
        track closest = disk_queue[0];
        int index = 0;
        for (size_t i = 1; i < disk_queue.size(); i++)
        {
            if (abs(disk_queue[i].tnum - cur_track) < abs(closest.tnum - cur_track))
            {
                closest = disk_queue[i];
                index = i;
            }
        }
        // update cur_track
        cur_task = closest;
        cur_track = cur_task.tnum;
        // pop the track from the disk_queue
        disk_queue.erase(disk_queue.begin() + index);
        disk_data[cur_task.rid].pop_front();
        requester_map[cur_task.rid] = false;
        if (disk_data[cur_task.rid].size() == 0)
        {
            num_active = num_active - 1;
        }
        requesterCV.broadcast();
    }
    disklock.unlock();
}

void run(void *arg)
{
    for (size_t i = 0; i < (size_t)num_requesters; i++)
    {
        thread(reinterpret_cast<thread_startfunc_t>(requester), reinterpret_cast<void *>(i));
    }
    thread(reinterpret_cast<thread_startfunc_t>(servicer), nullptr);
}

int main(int argc, char *argv[])
{
    // receive arguments, and cretae the main thread using boot
    max_disk_queue = 3;
    num_requesters = 5;

    std::deque<int> tempdeque1;
    tempdeque1.push_back(785);
    tempdeque1.push_back(53);
    disk_data.push_back(tempdeque1);
    std::deque<int> tempdeque2;
    tempdeque2.push_back(350);
    tempdeque2.push_back(914);
    disk_data.push_back(tempdeque2);
    std::deque<int> tempdeque3;
    tempdeque3.push_back(827);
    tempdeque3.push_back(567);
    disk_data.push_back(tempdeque3);
    std::deque<int> tempdeque4;
    tempdeque4.push_back(302);
    tempdeque4.push_back(230);
    disk_data.push_back(tempdeque4);
    std::deque<int> tempdeque5;
    tempdeque5.push_back(631);
    tempdeque5.push_back(11);
    disk_data.push_back(tempdeque5);

    for (size_t i = 0; i < disk_data.size(); i++)
    {
        if (disk_data[i].size() > 0)
        {
            num_active = num_active + 1;
        }
        requester_map.insert({i, false});
    }
    cpu::boot(1, reinterpret_cast<thread_startfunc_t>(run), nullptr, false, false, 0);
}
