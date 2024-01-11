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
    p1[5] = 'a';
    std::cout << p2[5] <<std::endl;
    char *f2= (char *) vm_map(nullptr, 0);
    f2[1] = 'a';
    char *f3 = (char *) vm_map(nullptr, 0);
    f3[1] = 'b';
    p3[5] = 'c';
    f2[1] = 'b';
}