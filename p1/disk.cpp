#include <iostream>
#include <deque>
#include <string>
#include "thread.h"
#include "disk.h"
#include <fstream>
#include <unordered_map>
#include<unistd.h>               // for linux 
#include <vector>
#include <algorithm>


// using std::cout;
// using std::endl;


// mutex mutex_lock;
// cv manager_cv;
// cv request_cv;

// int disk_size = 0;
// std::deque<std::pair<int,int>> disk_queue; // first value, second thread id
// int num_arguments = 0;
// std::vector<std::deque<int>> inputs;
// int requesters = 0;
// std::pair<int,int> curr_process = std::make_pair (0,-10);
// std::unordered_map<int, bool> thread_tracker; // first thread, second value


// void manage_thread(void *a){
//     mutex_lock.lock();
//     while(true){
//         if (requesters == 0){
//             break;
//         }
//         bool con1 = (requesters < disk_size && static_cast<int>(disk_queue.size()) < requesters);
//         bool con2 = (requesters >= disk_size && static_cast<int>(disk_queue.size()) < disk_size);

//         while(con1 || con2){
//             manager_cv.wait(mutex_lock);
//             con1 = (requesters < disk_size && static_cast<int>(disk_queue.size()) < requesters);
//             con2 = (requesters >= disk_size && static_cast<int>(disk_queue.size()) < disk_size);
//         }
//         if (requesters == 0){
//             break;
//         }
//         //find the closet std::vector<std::pair<int,int>> disk_queue;
//         int min = abs(disk_queue.front().first-curr_process.first);
//         int min_index = 0;
//         int min_thread = disk_queue.front().second;
//         for (int i = 0; i < static_cast<int>(disk_queue.size()); i++){
//             if(abs(disk_queue[i].first-curr_process.first) < min){
//                 min = abs(disk_queue[i].first-curr_process.first);
//                 min_index = i;
//                 min_thread = disk_queue[i].second;
//             }
//         }
//         //update the current process
//         curr_process.first = disk_queue[min_index].first;
//         curr_process.second = disk_queue[min_index].second;
//         //remove the process from the queue
//         disk_queue.erase(disk_queue.begin() + min_index);
//         inputs[min_thread].pop_front();
//         thread_tracker[min_thread] = false;
//         print_service(min_thread, curr_process.first);
//         request_cv.broadcast();
//     }
//     mutex_lock.unlock();
// }

// void request_thread(void *a){
//     mutex_lock.lock();
//     auto id =  reinterpret_cast<uintptr_t> (a);
//     while (true){
//         if (inputs[id].empty()){
//             requesters--;
//             manager_cv.signal();
//             break;
//         }
//         bool con1 = ((requesters < disk_size) && thread_tracker[id]);
//         bool con2 = ((requesters >= disk_size) && thread_tracker[id]);
//         bool con3 = ((requesters >= disk_size) && static_cast<int>(disk_queue.size()) == disk_size);

//         while(con1||con2||con3){
//             request_cv.wait(mutex_lock);
//             con1 = ((requesters < disk_size) && thread_tracker[id]);
//             con2 = ((requesters >= disk_size) && thread_tracker[id]);
//             con3 = ((requesters >= disk_size) && (static_cast<int>(disk_queue.size()) == disk_size));
//         }
//         if (inputs[id].empty()){
//             requesters--;
//             manager_cv.signal();
//             break;
//         }
//         //add the request to the queue
//         disk_queue.push_back(std::make_pair(inputs[id].front(), id));
//         thread_tracker[id] = true;
//         print_request(id, inputs[id].front());
//         if(static_cast<int>(disk_queue.size()) == disk_size && requesters >= disk_size){
//             manager_cv.signal();
//         }
//         else if((static_cast<int>(disk_queue.size()) == requesters) && (requesters < disk_size)){
//             manager_cv.signal();
//         }
//     }
//     mutex_lock.unlock();
// }

// void server_thread(void* a){
//     auto argv =  reinterpret_cast<char**> (a);
//     disk_size = atoi(argv[1]);

//     //input 
//     requesters = num_arguments - 2;
//     for(int i = 0; i < num_arguments - 2; i++){
//         std::deque<int> temp;
//         std::fstream my_file;
//         my_file.open(argv[i + 2]);
//         if (!my_file) {
//             cout << "No such file";
//         }
//         else {
//             int request_number;
//             while (1) {
//                 my_file >> request_number;
//                 request_number = static_cast<int>(request_number);
//                 if (!my_file)
//                     break;
//                 else{
//                     temp.push_back(request_number);
//                 }
//             }
//         }
//         inputs.push_back(temp);
//         my_file.close();
//     }

//     //call manager thread
//     thread manager(reinterpret_cast<thread_startfunc_t>(manage_thread), reinterpret_cast<void *>(argv));
//     //call request thread
//     for (uintptr_t i = 0; i < static_cast<uintptr_t>(requesters); i++){
//         thread single_request(reinterpret_cast<thread_startfunc_t>(request_thread), reinterpret_cast<void *>(i));
//   }

// }

mutex m;
cv cv1;

void func(void *a)
{
    char id = *(static_cast<char *>(a));
    m.lock();
    std::cout << id << " locked" << std::endl;
    cv1.wait(m);
    std::cout << id << " wait end" << std::endl;
    m.unlock();
    std::cout << id << " unlocked" << std::endl;
}

void funcReleaseSignal(void *a)
{
    for (int i = 0; i < 4; i++)
    {
        std::cout << "relseasing!!" << std::endl;
        cv1.signal();
    }
}

void funcReleaseBroadcasting(void *a)
{
    std::cout << "broadcasting!!" << std::endl;
    cv1.broadcast();
}

void run(void *arg)
{
    cv1.signal();
    cv1.broadcast();
    thread tA(reinterpret_cast<thread_startfunc_t>(func), const_cast<void *>(static_cast<const void *>("A")));
    thread tB(reinterpret_cast<thread_startfunc_t>(func), const_cast<void *>(static_cast<const void *>("B")));
    thread tC(reinterpret_cast<thread_startfunc_t>(func), const_cast<void *>(static_cast<const void *>("C")));
    thread tD(reinterpret_cast<thread_startfunc_t>(func), const_cast<void *>(static_cast<const void *>("D")));
    thread tE(reinterpret_cast<thread_startfunc_t>(funcReleaseSignal), const_cast<void *>(static_cast<const void *>("E")));
    thread tF(reinterpret_cast<thread_startfunc_t>(func), const_cast<void *>(static_cast<const void *>("F")));
    thread tG(reinterpret_cast<thread_startfunc_t>(func), const_cast<void *>(static_cast<const void *>("G")));
    thread tH(reinterpret_cast<thread_startfunc_t>(func), const_cast<void *>(static_cast<const void *>("H")));
    thread tI(reinterpret_cast<thread_startfunc_t>(func), const_cast<void *>(static_cast<const void *>("I")));
    thread tK(reinterpret_cast<thread_startfunc_t>(funcReleaseBroadcasting), const_cast<void *>(static_cast<const void *>("K")));
}

int main(int argc, char *argv[]) {
    // num_arguments = argc;
    cpu::boot(reinterpret_cast<thread_startfunc_t>(run), reinterpret_cast<void *>(argv), 500000);
    return 0;
}