#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using std::cout;
using std::endl;

int main()
{


    char *t1 = (char *) vm_map(nullptr, 0); //0x600010000


    /* Write the name of the file that will be mapped */
    strcpy(t1, "data1.bin");

    char* t2 = (char *) vm_map(nullptr, 0);
    t2[65535]= 'd';
    char* t3 = (char *)vm_map(nullptr,0);
    strcpy(t1, "ata1.bin");

    char* file2 = (char *)vm_map(t2+65535,0);
    char* file3 = (char*)vm_map(t1, 0);

    if (fork()){
        vm_yield();
        char* file1 = (char *)vm_map(t1,0);
        for (int i = 0; i < 1937; i++){
            vm_yield();
            file1[i] = i;
            std::cout<<file3[i+1]<<std::endl;
            vm_yield();
            file2[i] = 0;
            vm_yield();
        }

    }
    else{
        char* file2 = (char *)vm_map(t2+65535,0);
        vm_yield();
        for (int i = 1937; i >= 0; i--){
            vm_yield();
            file2[i] = i;
            vm_yield();
            std::cout<<file2[i+1]<<std::endl;
            vm_yield();
            file3[i] = '5';
        }

    }

}