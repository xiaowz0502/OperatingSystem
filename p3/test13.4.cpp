#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using std::cout;


int main() { 
    /* 4 pages of physical memory in the system */
        char *page0 = (char *) vm_map(nullptr, 0);
        page0[1] = 'a';
        char *page1 = (char *) vm_map(nullptr, 0);
        page1[2] = 'a';
        char *page2 = (char *) vm_map(nullptr, 0);
        page2[2] = 'a';
        char *page3 = (char *) vm_map(nullptr, 0);
    if (fork()) { // parent
        vm_yield();
        page3[0] = 'b';
    } 
    else { // child
        page3[0] = 'b';
    }
}
