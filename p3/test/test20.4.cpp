#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using std::cout;
using std::endl;

int main()
{
    //block over range

    char* page1 = (char*)vm_map(nullptr, 1);
    strcpy(page1, "lampson83.txt");
    char *p = (char *) vm_map (page1, 1);

    /* Print the first part of the paper */
    for (unsigned int i=0; i<65536; i++) {
	    cout << p[i];
    }

}



