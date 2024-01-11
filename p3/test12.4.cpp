#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using std::cout;
using std::endl;

int main()
{
    //swap back page that used to store filename, state 100
    char *filename = (char *) vm_map(nullptr, 0);

    //store the filename, filename have state 111
    strcpy(filename, "lampson83.txt");

    //file1 state 100
    char *file1 = (char *) vm_map(filename, 0);

    //filename state 111
    strcpy(filename, "data1.bin");

    //file2 state 100
    char *file2 = (char *) vm_map(filename, 0);

    //file1 state 110
    cout << file1[0] << endl;

    //read again state 110
    cout << file1[1] << endl;

    //file1 state 111
    file1[0] = '1';

    file1[1] = '1';

    cout << file1[0] << endl;

    //file2 state 111
    file1[2] = '2';

    //filename state 111
    strcpy(filename, "data2.bin");

    //file3 state 100, filename to the disk with state 000
    char *file3 = (char *) vm_map(filename, 0);
    
    //file4 state 000, also in disk
    char *file4 = (char *) vm_map(filename, 0);

    //bring filename to phymem, file2 to disk
    strcpy(filename, "lampson83.txt");

    //bring file1 from disk to phymem
    file1[1] = '1';

}



