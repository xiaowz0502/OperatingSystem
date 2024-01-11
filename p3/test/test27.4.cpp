#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using std::cout;
using std::endl;

int main()
{
    // Map the same file to three different virtual memory regions
    char *filename = (char *) vm_map(nullptr, 0);
    strcpy(filename, "lampson83.txt");

    char *region1 = (char *) vm_map(filename, 0);
    cout << "region1: " << region1 << endl;

    char *region2 = (char *) vm_map(filename, 0);
    cout << "region2: " << region2 << endl;

    char *region3 = (char *) vm_map(filename, 0);
    cout << "region3: " << region3 << endl;

    filename[0] = 'K';

    if (fork()) { // parent
        filename[1] = 'P';
        vm_yield();
    } else { // child
        filename[1] = 'C';
    }

    // Modify the contents of the first virtual memory region
    cout << "region1 contents before modification: " << region1 << endl;
    region1[0] = 'A';
    cout << "region1 contents after modification: " << region1 << endl;

    // Check that the contents of the other virtual memory regions have been correctly updated
    cout << "region2 contents after modification: " << region2 << endl;
    cout << "region3 contents after modification: " << region3 << endl;


        // Map the same file to three different virtual memory regions
    char *filename1 = (char *) vm_map(nullptr, 0);
    strcpy(filename1, "lampson83.txt");

    char *region4 = (char *) vm_map(filename, 0);
    cout << "region1: " << region4 << endl;

    char *region5 = (char *) vm_map(filename, 0);
    cout << "region2: " << region5 << endl;

    char *region6 = (char *) vm_map(filename, 0);
    cout << "region3: " << region6 << endl;

    filename1[0] = 'K';

    if (fork()) { // parent
        filename1[1] = 'P';
        vm_yield();
    } else { // child
        filename1[1] = 'C';
    }

    // Modify the contents of the first virtual memory region
    cout << "region1 contents before modification: " << region4 << endl;
    region4[0] = 'A';
    cout << "region1 contents after modification: " << region4 << endl;

    // Check that the contents of the other virtual memory regions have been correctly updated
    cout << "region2 contents after modification: " << region5 << endl;
    cout << "region3 contents after modification: " << region6 << endl;


    return 0;
}
