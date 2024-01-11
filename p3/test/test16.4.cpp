#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"
#include <assert.h>

using std::cout;


int main() { /* 4 pages of physical memory in the system */
    char *filename = (char *) vm_map(nullptr, 0);
    strcpy(filename, "lampson83.txt");
    char *fb_page = (char *) vm_map(filename, 0);
    if (fork()) { // parent
        vm_yield();
        fb_page[4] = 'B';
        vm_yield();
        char *filename3 = (char *) vm_map(filename, 1);
        filename3[5] = 'b';
    } else { // child
        fb_page[3] = 'B';
        vm_yield();
        char *filename2 = (char *) vm_map(nullptr, 0);
        filename2[1] = 'A';
        char *fb_page2 = (char *) vm_map(filename, 1);
        fb_page2[3] = 'C';
    }
    fb_page[1] = 'a';
    char *fb_page2 = (char *) vm_map(filename, 0);
    strcpy(filename, "data1.bin");
    char *afterpage1 = (char *) vm_map(filename, 1);
    strcpy(filename, "data2.bin");
    char *afterpage2 = (char *) vm_map(filename, 1);
    strcpy(filename, "data3.bin");
    char *afterpage3 = (char *) vm_map(filename, 1);

    if (fork()) { // parent
        vm_yield();
        afterpage1[4] = 'B';
        vm_yield();
        afterpage2[5] = 'b';
        fb_page[0] = 'c';
        
    } else { // child
        afterpage1[3] = 'B';
        vm_yield();
        afterpage3[1] = 'A';
        fb_page[3] = 'C';
    }
}
