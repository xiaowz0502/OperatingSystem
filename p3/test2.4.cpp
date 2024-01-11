#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using std::cout;

int main()
{
   if (fork()){
    char *filename = (char *) vm_map(nullptr, 0); 

    /* Write the name of the file that will be mapped */
    strcpy(filename, "lampson83.txt");

    /* Map a page from the specified file */
    char *p = (char *) vm_map (filename, 0);
    p[1] = 'a';
   }
   else{
        char *filename1 = (char *) vm_map(nullptr, 0); 

    /* Write the name of the file that will be mapped */
        strcpy(filename1, "lampson83.txt");

    /* Map a page from the specified file */
        char *p1 = (char *) vm_map (filename1, 0);
   }
}