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


    char *filename1 = (char *) vm_map(nullptr, 0);
    strcpy(filename1, "lampson83.txt");
    char *p4 = (char *) vm_map (filename, 0);
    char *p5 = (char *) vm_map (filename, 0);
    char *p6 = (char *) vm_map (filename, 0);
    p4[5] = 'a';
    std::cout << p5[5] <<std::endl;
    char *f7= (char *) vm_map(nullptr, 0);
    f7[1] = 'a';
    char *f8 = (char *) vm_map(nullptr, 0);
    f8[1] = 'b';
    p6[5] = 'c';
    f7[1] = 'b';
}