#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using std::cout;
using std::endl;

int main()
{
    //100
    char *swap1 = (char *) vm_map(nullptr, 0);

    //110
    cout << swap1[0] << endl;
    cout << swap1[1] << endl;

    //111
    swap1[0] = '1';
    cout << swap1[0] << endl;
    swap1[0] = '1';
    cout << swap1[0] << endl;

    //100
    char *swap2 = (char *) vm_map(nullptr, 0);
    swap2[0] = '2';

    //100
    char *swap3 = (char *) vm_map(nullptr, 0);
    swap3[0] = '3';

    //all swap123 should have res, ref, dir bit of 111 right now

    //100
    char *swap4 = (char *) vm_map (nullptr, 0);

    //trigger clock algorithm, swap1 should have 000 state and go into disk
    //swap23 should have state 101, swap4 111
    swap4[0] = '4';

    //swap1 should be grabed from disk and save to physical memory, swap2 should go to disk
    //swap1 should have state 111, swap2 000, swap34 101
    swap1[1] = '1';

    //swap2 should be grabed from disk, swap3 should go to disk
    //swap 2 should have state 110, swap3 000, swap13 101
    cout << swap2[0] << endl;
}