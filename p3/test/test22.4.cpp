#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using std::cout;

int main()
{
    char *filename = (char *) vm_map(nullptr, 0);
    strcpy(filename, "lampson83.txt");
    char *p1 = (char *) vm_map (filename, 0);
    char *p2 = (char *) vm_map (filename, 0);
    char *p3 = (char *) vm_map (filename, 0);

    if(fork()){
        vm_yield();
        p2[1] = 'b';
        p1[5] = 'a';
        vm_yield();
    }
    else{
        if (fork()){
            vm_yield();
            p1[5] = 'a';
            std::cout << p2[5] <<std::endl;
            vm_yield();
            char *f2= (char *) vm_map(nullptr, 0);
            f2[1] = 'a';
            exit(0);
        }
        else{
            p1[5] = 'a';
            std::cout << p2[5] <<std::endl;
            char *f2= (char *) vm_map(nullptr, 0);
            f2[1] = 'a';
            vm_yield();
            char *f3 = (char *) vm_map(nullptr, 0);
            f3[1] = 'b';
            p3[5] = 'c';
            vm_yield();
            f2[1] = 'b';
            exit(0);
        }
    }
}