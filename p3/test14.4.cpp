#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

int main()
{
    /* Allocate swap-backed page from the arena */
    char *page1 = (char *) vm_map(nullptr, 0);
    page1[65535] = 'l';

    char *page2 = (char *) vm_map(nullptr, 0);
    strcpy(page2, "ampson83.txt");

    char *page3 = (char *) vm_map(nullptr, 0);
    page3[5] = 'a';

    char *page4 = (char *) vm_map(nullptr, 0);
    page4[6] = 'a';

    char *page5 = (char *) vm_map(nullptr, 0);
    page5[6] = '7';

    char* filename = page1 + 65535;
    char* file = (char *) vm_map(filename, 0);
    file[6] = 'c';

}

