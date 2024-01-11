#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"
#include <assert.h>

using std::cout;


int main() { /* 4 pages of physical memory in the system */
    char *page1 = (char *) vm_map(nullptr, 0);
    page1[65535] = 'l';

    char *page2 = (char *) vm_map(nullptr, 0);
    strcpy(page2, "ampson83.txt");

    if (fork()) { // parent
        char* page3 = (char*) vm_map(nullptr, 0);
        page3[5] = 'b';
        vm_yield();
        page1[3] = 'l';
        char* filename = page1 + 65535;
        char* page5 = (char*)vm_map(filename,1);
        vm_yield();
        page5[5] = 'b'; 

    } else { // child
       char* page4 = (char*) vm_map(nullptr, 0);
       page4[5] = 'b';
       vm_yield();
       char* filename2 = page1 + 65535;
       char* page6 = (char*)vm_map(filename2,1);
       page6[2] = 'b'; 

    }
}
