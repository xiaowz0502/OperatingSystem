#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using std::cout;
using std::endl;

int main()
{
    char *swap1 = (char *) vm_map(nullptr, 0);
    cout << swap1[0] << endl;
    cout << swap1[1] << endl;
    swap1[0] = '1';

    char *swap2 = (char *) vm_map(nullptr, 0);
    swap2[0] = '2';

    char *swap3 = (char *) vm_map(nullptr, 0);
    swap3[0] = '3';

    char *swap4 = (char *) vm_map (nullptr, 0);

    //swap1 go to disk
    swap4[0] = '4';

    //swap1 back to physical memory, swap2 to disk
    cout << swap1[0] << endl;

    //swap2 back tp physical memory, swap3 to disk
    cout << swap2[0] << endl;

    //swap3 back tp physical memory, swap4 to disk
    cout << swap3[0] << endl;

    //make sure that they are referenced before next clock algorithm
    cout << swap1[0] << endl;
    cout << swap2[0] << endl;
    cout << swap3[0] << endl;

    //swap4 back tp physical memory, swap1 to disk
    cout << swap4[0] << endl;
    cout << swap1[0] << endl;
    cout << swap2[0] << endl;
    cout << swap3[0] << endl;
}