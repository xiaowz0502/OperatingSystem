#include "vm_pager.h"
#include <unordered_map>
#include <deque>
#include <utility> 
#include <iostream>
#include <string> 
#include <cstring>
#include <iterator>
#include <algorithm>
#include <vector>


struct page{
    pid_t pid; //process id
    bool dir; //dirty bit
    bool ref; //reference bit
    bool res; //resident bit
    page_table_entry_t* phy_page;
    std::deque<std::pair<pid_t, int>> refer_list;
    std::pair<char*, unsigned int> fb; //filename， block 
};


struct virtual_memory_table {
    page* table[VM_ARENA_SIZE/VM_PAGESIZE];
    int num_swap;//total swap pages
    int size; //toal pages
    // int num_file;
};

struct vir_phy{
    virtual_memory_table *vir;
    page_table_t *phy; // physical memory, read_bit, write_bit
};


std::unordered_map<pid_t, vir_phy*> pro_map; //process map
std::unordered_map<long long, page*> page_map; //page map

struct MyKeyHash {
    std::size_t operator()(const std::pair<std::string, int>& k) const {
        return std::hash<std::string>()(k.first) ^ std::hash<int>()(k.second);
    }
};

std::unordered_map<std::pair<std::string, unsigned int>, page*, MyKeyHash> file_track;// file_back pages track



//swap back file available queue
std::deque<unsigned int> swap_queue;

//physical memory available queue
std::deque<unsigned int> avil_queue;

//clock algorithm deque
std::deque<page*> clock_deque;
size_t cur_cid; //current pointer of the clock

int MEMORY_PAGES;
int SWAP_BLOCKS;
int cur_swap;

pid_t cur_pid; //current process id


void remove_clock(std::deque<page*>::iterator it) {
    //假设我们的要remove的page一定存在在这个deque里
    if (it < (clock_deque.begin() + cur_cid)) {
        cur_cid--;
    }

    if (it == clock_deque.end() - 1) {
        cur_cid = 0;
    }
    
    clock_deque.erase(it);
}

int clock_algorithm() {
    size_t i = cur_cid;
    while (true) {
        if (clock_deque[i]->ref == false) {
            //erase the evict page_id* and write the file into the disk and clear those bits
            clock_deque[i]->phy_page->read_enable = 0;
            clock_deque[i]->phy_page->write_enable = 0;

            //get the physical page id and release the space
            unsigned int phy_index = clock_deque[i]->phy_page->ppage;
            avil_queue.push_back(phy_index);
            
            if (clock_deque[i]->dir == true) {
                char* addr = (char*)(vm_physmem) + (phy_index * VM_PAGESIZE);
                void* target = reinterpret_cast<void*>(addr);
                if (clock_deque[i]->fb.first != nullptr){

                    //==================所有的char*都要注意不然就只是local variable======================================
                    if (file_write(clock_deque[i]->fb.first, clock_deque[i]->fb.second, target) == -1){
                        avil_queue.pop_back();
                        return -1;
                    }
                    // file_write(clock_deque[i]->fb.first, clock_deque[i]->fb.second, target);
                    for (unsigned int j = 0; j < clock_deque[i]->refer_list.size(); j++) {
                        pid_t temp_pid = clock_deque[i]->refer_list[j].first;
                        unsigned int index = clock_deque[i]->refer_list[j].second;
                        pro_map[temp_pid]->phy->ptes[index].read_enable = 0;
                        pro_map[temp_pid]->phy->ptes[index].write_enable = 0;
                    }
                }
                else{
                    // it is a swap page, firsly get available swap spot
                    if (file_write(nullptr, clock_deque[i]->fb.second, target) == -1){
                        return -1;
                    }
                    // file_write(nullptr, clock_deque[i]->fb.second, target);
                }
            }
            //file write and change the res, ref, dir to 0
            clock_deque[i]->dir = false;
            clock_deque[i]->ref = false;
            clock_deque[i]->res = false;
            remove_clock(clock_deque.begin() + i);
            return 0;
        }

        clock_deque[i]->ref = false;

        //change the bits read and write bit change to 0
        clock_deque[i]->phy_page->read_enable = 0;
        clock_deque[i]->phy_page->write_enable = 0;
        if (clock_deque[i]->fb.first != nullptr) {
            for (unsigned int j = 0; j < clock_deque[i]->refer_list.size(); j++) {
                pid_t temp_pid = clock_deque[i]->refer_list[j].first;
                unsigned int index = clock_deque[i]->refer_list[j].second;
                pro_map[temp_pid]->phy->ptes[index].read_enable = 0;
                pro_map[temp_pid]->phy->ptes[index].write_enable = 0;
            }
        }


        if ((clock_deque.begin() + i) == (clock_deque.end() - 1)) {
            i = 0;
        } else {
            i++;
        }
    }
}

void insert_clock(page* pg) {
    //update clock queue
    //pointer可能有问题
    if (cur_cid == 0) {
        clock_deque.push_back(pg);
        return;
    }
    if (find(clock_deque.begin(), clock_deque.end(), pg) == clock_deque.end()) {
        clock_deque.insert(clock_deque.begin() + cur_cid, pg);
        if (cur_cid == clock_deque.size()) {
            cur_cid = 0;
        } else {
            cur_cid++;
        }
    }
}

void vm_init(unsigned int memory_pages, unsigned int swap_blocks) {
    MEMORY_PAGES = memory_pages;
    SWAP_BLOCKS = swap_blocks; 
    cur_swap = 0; 
    cur_cid = 0;

    for (int i = 0; i < SWAP_BLOCKS; i++){
        swap_queue.push_back(i); 
    }
    
    for (int i = 1; i < MEMORY_PAGES; i++){
        avil_queue.push_back(i); 
    }

    //set the first all zero 
    char* first_page = (char*)(vm_physmem);
    for (unsigned int i = 0; i < VM_PAGESIZE; i++){
        first_page[i] = 0;
    }
}


void initialize_empty_arena(pid_t pid) {
    virtual_memory_table* new_table = new virtual_memory_table;
    new_table->num_swap = 0;
    new_table->size = 0;
    for (unsigned int i = 0; i < VM_ARENA_SIZE / VM_PAGESIZE; i++) {
        new_table->table[i] = nullptr;
    }
    //在这里我们没有对page_table_entry_t的值进行修改，后续要每次都check size， 不然会有undefined behavior
    page_table_t* new_phy_table = new page_table_t;

    vir_phy* new_vir_phy = new vir_phy;
    new_vir_phy->vir = new_table;
    new_vir_phy->phy = new_phy_table;
    for (unsigned int i = 0; i < VM_ARENA_SIZE / VM_PAGESIZE; i++) {
        new_vir_phy->phy->ptes[i].read_enable = 0;
        new_vir_phy->phy->ptes[i].write_enable = 0;
        new_vir_phy->phy->ptes[i].ppage = 0;
    }
    pro_map[pid] = new_vir_phy;
}

//create a deep copy for parent process's vir_phy
int vm_create(pid_t parent_pid, pid_t child_pid){
    //
    initialize_empty_arena(child_pid);
    // If there are not enough free swap blocks, vm_create should return -1=====================
    return 0;

}


void vm_switch(pid_t pid){
    page_table_base_register = pro_map[pid]->phy;
    cur_pid = pid;
}

int handle_rw(bool res, page* p, unsigned int page_id, bool write_flag) {
    if (!res) {
        //从disk挪到physical memory
        char *buf;
        if (avil_queue.empty()) {
            if (clock_algorithm() == -1){
                return -1;
            }
        }
        insert_clock(p);

        unsigned int avail = avil_queue.front();
        avil_queue.pop_front();
        buf = reinterpret_cast<char*>(vm_physmem) + static_cast<int>(avail * VM_PAGESIZE);
        
        if (p->fb.first != nullptr){
            if (file_read(p->fb.first, p->fb.second, reinterpret_cast<void*>(buf)) == -1){
                avil_queue.push_front(avail);
                return -1;
            }
            for (unsigned int i = 0; i < p->refer_list.size(); i++) {
                pid_t temp_pid = p->refer_list[i].first;
                unsigned int index = p->refer_list[i].second;
                pro_map[temp_pid]->phy->ptes[index].ppage = avail;
                if (write_flag) {
                    pro_map[temp_pid]->phy->ptes[index].write_enable = 1;
                }
                pro_map[temp_pid]->phy->ptes[index].read_enable = 1;
            }
            // std::string filename(p->fb.first);
            // std::pair<std::string, int> myPair = std::make_pair(filename, static_cast<int>(p->fb.second));
            // file_track[myPair] = avail;
        } 
        else{
            if (file_read(nullptr,p->fb.second, reinterpret_cast<void*>(buf)) == -1){
                avil_queue.push_front(avail);
                return -1;
            }
            // file_read(nullptr,p->fb.second, reinterpret_cast<void*>(buf));

        }
        p->res = true;
        p->phy_page->ppage = avail;
    }

    if (p->fb.first == nullptr && p->phy_page->ppage == 0) {
        if (write_flag) {
            //pop avail_queue for physical memory
            if (avil_queue.empty()) {
                if (clock_algorithm() == -1){
                    return -1;
                }
            }
            insert_clock(p);
            
            unsigned int avil2 = avil_queue.front();
            avil_queue.pop_front();

            //把arena里的mapping改到新的phyical address
            p->phy_page->ppage = avil2;
            char* tar = (char*)(vm_physmem) + static_cast<int>(avil2 * VM_PAGESIZE);
            for (unsigned int i = 0 ; i <VM_PAGESIZE; i++){
                tar[i] = 0;
            }
        }
    }

    if (write_flag) {
        //swap back file copy
        pro_map[cur_pid]->vir->table[page_id]->dir = true;
        pro_map[cur_pid]->vir->table[page_id]->phy_page->write_enable = 1;
    }
    if(!write_flag && pro_map[cur_pid]->vir->table[page_id]->dir == true){
        pro_map[cur_pid]->vir->table[page_id]->phy_page->write_enable = 1;
    }
    pro_map[cur_pid]->vir->table[page_id]->ref = true;
    pro_map[cur_pid]->vir->table[page_id]->phy_page->read_enable = 1;

    if (p->fb.first != nullptr) {
        for (unsigned int i = 0; i < p->refer_list.size(); i++) {
            pid_t temp_pid = p->refer_list[i].first;
            unsigned int index = p->refer_list[i].second;
            if (write_flag) {
                pro_map[temp_pid]->phy->ptes[index].write_enable = 1;
            }
            pro_map[temp_pid]->phy->ptes[index].read_enable = 1;
        }
    }
    return 0;
}

int vm_fault(const void* addr, bool write_flag){
    //calculate the number of page;
    uintptr_t uintptr_addr = reinterpret_cast<uintptr_t>(addr);

    if (uintptr_addr < 0){
        return -1;
    }

    uintptr_t bits0to19 = (uintptr_addr - reinterpret_cast<uintptr_t>(VM_ARENA_BASEADDR));
    unsigned int page_id = static_cast<unsigned int>(bits0to19) / VM_PAGESIZE; 
    

    //the address is to an invalid page
    if (page_id >= static_cast<unsigned int>(pro_map[cur_pid]->vir->size) || page_id < 0 ||page_id >= (VM_ARENA_SIZE / VM_PAGESIZE)){
        return -1;
    }

    bool res = pro_map[cur_pid]->vir->table[page_id]->res;
    page* p =  pro_map[cur_pid]->vir->table[page_id];

    if (handle_rw(res, p, page_id, write_flag) == -1) {
        return -1;
    }
    return 0;
}


void vm_destroy(){
    //for loop virtual table.
    auto var = pro_map[cur_pid]->vir->table;

    //delete clock algorithm

    for (unsigned int i = 0; i < static_cast<unsigned int>(pro_map[cur_pid]->vir->size); i++) {
        // If swap-backed, push swap_queue, delete the page*
        if (var[i]->fb.first == nullptr) {
            swap_queue.push_back(var[i]->fb.second);

            if (var[i]->res){
                if (var[i]->phy_page->ppage != 0) {
                    avil_queue.push_back(var[i]->phy_page->ppage);
                }
            }

            auto it_clock = find(clock_deque.begin(), clock_deque.end(), var[i]);
            if (it_clock != clock_deque.end()){
                remove_clock(it_clock);
                // clock_deque.erase(clock_deque.begin() + i);
            }

            delete var[i]->fb.first;
            delete var[i];
            cur_swap--;
        }
        else{
            std::string cpp_string(var[i]->fb.first);
            page_table_entry_t* phy_table_entry = new page_table_entry_t;
            phy_table_entry->ppage = var[i]->phy_page->ppage;
            phy_table_entry->read_enable = var[i]->phy_page->read_enable;
            phy_table_entry->write_enable = var[i]->phy_page->write_enable;
            file_track[{cpp_string,var[i]->fb.second}]->phy_page = phy_table_entry;

            
            // If file back, remove from reference list, delete the page*
            auto it = std::find_if(
                var[i]->refer_list.begin(),
                var[i]->refer_list.end(),
                [&](const std::pair<pid_t, int>& p)
                { return p.first == cur_pid && static_cast<unsigned int>(p.second) == i; });

            if (it != var[i]->refer_list.end()) {
                var[i]->refer_list.erase(it);
            }
        }
    }
    delete pro_map[cur_pid]->phy;
    delete pro_map[cur_pid]->vir;
    pro_map.erase(cur_cid);
    delete pro_map[cur_pid];


}


void *vm_map(const char *filename, unsigned int block){
    if (filename == nullptr && cur_swap == SWAP_BLOCKS) { //check if it exceeds swap blocks
        return nullptr;
    }
    
    
    if(static_cast<unsigned int>(pro_map[cur_pid]->vir->size + 1) > (VM_ARENA_SIZE / VM_PAGESIZE)){ //check if exceed arena size
        return nullptr;
    }
    
    
    //还没有复制到array还有map里面去
    
    //if swap back, map to the first physical memory page
    if (filename == nullptr) {
        page *pg = new page;
        pg->res = true;
        pg->ref = false; //???????? should we change it to true
        pg->dir = false;
        pro_map[cur_pid]->vir->table[pro_map[cur_pid]->vir->size] = pg;
        pg->phy_page = pro_map[cur_pid]->phy->ptes + pro_map[cur_pid]->vir->size;
        pg->phy_page->ppage = 0;
        pg->phy_page->read_enable = 1; //swap back page的read bit到底应该是1还是0？？？？？？？？？
        pg->phy_page->write_enable = 0;

        pg->fb.first = nullptr;
        pg->fb.second = swap_queue.front();
        pro_map[cur_pid]->vir->num_swap++;
        cur_swap++;
        swap_queue.pop_front();
    } 



    else { //if file-backed
        
        uintptr_t uintptr_addr = reinterpret_cast<uintptr_t>((char *)filename);

        if (uintptr_addr < reinterpret_cast<uintptr_t>(VM_ARENA_BASEADDR)){ //check if the filename address given is not in the virtual address space
            return nullptr;
        }

        uintptr_t bits0to19 = (uintptr_addr - reinterpret_cast<uintptr_t>(VM_ARENA_BASEADDR));
        unsigned int page_id = static_cast<unsigned int>(bits0to19) / VM_PAGESIZE; 

          //the address is to an invalid page
        if (page_id >= static_cast<unsigned int>(pro_map[cur_pid]->vir->size) || page_id < 0){
            return nullptr;
        }

        // uintptr_t bits_20_31 = (uintptr_addr - reinterpret_cast<uintptr_t>(VM_ARENA_BASEADDR));
        unsigned int offset = static_cast<unsigned int>(bits0to19) % VM_PAGESIZE; 

        if (offset < 0){
            return nullptr;
        }

        char curr;
        std::string f_n = "";
        
        while (true){
            bool res = pro_map[cur_pid]->vir->table[page_id]->res;
            page* cur_pg = pro_map[cur_pid]->vir->table[page_id];

            if (handle_rw(res, cur_pg, page_id, false) == -1){
                return nullptr;
            }

            //for loop i = offset, i <
            char* start = (char*)(vm_physmem) + (pro_map[cur_pid]->vir->table[page_id]->phy_page->ppage * VM_PAGESIZE);
            for (unsigned int i = offset; i < VM_PAGESIZE; i++) {
                if (start[i] == '\0'){
                    curr = '\0';
                    break;
                }
                curr =start[i];
                f_n += curr;
            }

            //if break while find the terminate null character
            if ( curr == '\0'){
                // f_n += curr;
                break;
            }
            //enter next page, update info
            page_id += 1;
            if(static_cast<unsigned int>(page_id) >= (VM_ARENA_SIZE / VM_PAGESIZE) ||page_id > static_cast<unsigned int>(pro_map[cur_pid]->vir->size)){
                return nullptr;
            }
            offset = 0;
        } 


        //cast the string f_n to a char*
        
        char* temp = new char[f_n.length() + 1];
        strcpy(temp, f_n.c_str());

        //check if the file back page is in file_track
        if (file_track.find(std::make_pair(f_n, block)) != file_track.end()) {
            pro_map[cur_pid]->vir->table[pro_map[cur_pid]->vir->size] = file_track[std::make_pair(f_n, block)];
            pro_map[cur_pid]->vir->table[pro_map[cur_pid]->vir->size]->refer_list.push_back(std::make_pair(cur_pid, pro_map[cur_pid]->vir->size));
            pro_map[cur_pid]->phy->ptes[pro_map[cur_pid]->vir->size].ppage = file_track[std::make_pair(f_n, block)]->phy_page->ppage;
            // auto ppage = file_track[std::make_pair(f_n, block)]->phy_page->ppage;
            pro_map[cur_pid]->phy->ptes[pro_map[cur_pid]->vir->size].read_enable = file_track[std::make_pair(f_n, block)]->phy_page->read_enable;
            pro_map[cur_pid]->phy->ptes[pro_map[cur_pid]->vir->size].write_enable = file_track[std::make_pair(f_n, block)]->phy_page->write_enable;
        } else {
            pro_map[cur_pid]->phy->ptes[pro_map[cur_pid]->vir->size].read_enable = 0;
            pro_map[cur_pid]->phy->ptes[pro_map[cur_pid]->vir->size].write_enable = 0;
            // unsigned int avail = avil_queue.front();
            // avil_queue.pop_front();
            // pro_map[cur_pid]->phy->ptes[pro_map[cur_pid]->vir->size].ppage = avail;
            page *pg = new page;
            pg->res = false;
            pg->ref = false;
            pg->dir = false;
            pg->refer_list.push_back(std::make_pair(cur_pid, pro_map[cur_pid]->vir->size));
            pg->fb.first = temp;
            pg->fb.second = (unsigned int)block;
            pg->phy_page = pro_map[cur_pid]->phy->ptes + pro_map[cur_pid]->vir->size;
    
            pro_map[cur_pid]->vir->table[pro_map[cur_pid]->vir->size] = pg;

            std::string cpp_string(temp);
            file_track[std::make_pair(cpp_string, block)] = pg;

        }
    }
    // auto size = pro_map[cur_pid]->vir->size;
    pro_map[cur_pid]->vir->size++; //increment the number of pages in the arena
    // size = pro_map[cur_pid]->vir->size;
    return reinterpret_cast<void*>(reinterpret_cast<char*>(VM_ARENA_BASEADDR) + (pro_map[cur_pid]->vir->size - 1) * VM_PAGESIZE);
}