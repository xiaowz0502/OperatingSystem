#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"
#include <assert.h>

using std::cout;


int main() { /* 4 pages of physical memory in the system */
    if (fork()) { // parent
        char *filename = (char *) vm_map(nullptr, 0);
        strcpy(filename, "lampson83.txt");
        char *fb_page = (char *) vm_map(filename, 0);
        fb_page[0] = 'B';
        vm_yield();
    } else { // child
        char *filename1 = (char *) vm_map(nullptr, 0);
        strcpy(filename1, "lampson83.txt");
        char *fb_page = (char *) vm_map(filename1, 0);
        assert(fb_page[0] == 'B');
        fb_page[0] = 'H';
    }
}
