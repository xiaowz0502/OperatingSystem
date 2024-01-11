#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using std::cout;
using std::endl;

int main()
{
    char* p1 = (char*)vm_map(nullptr, 0);
    p1[1] = 'a';
    char* p2 = (char*)vm_map(nullptr, 0);
    p2[1] = 'a';
    char* p3 = (char*)vm_map(nullptr, 0);
    p3[1] = 'a';
    char* p4 = (char*)vm_map(nullptr, 0);
    p4[1] = 'a';
    char* p5 = (char*)vm_map(nullptr, 0);
    std::cout << p3[3] << std::endl;
    std::cout << p2[1] << std::endl;
    p3[2] = 'b';
}