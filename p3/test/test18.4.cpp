#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include "vm_app.h"

using namespace std;

int main(){
    int pid1, pid2, pid3;
    char *p1, *p2, *p3;
    int status;

    for (int i = 0; i < 5000; i++) {
        pid1 = fork();
        if (pid1 == 0) { // Child 1
            p1 = (char *) vm_map(nullptr, 0);
            strcpy(p1, "lampson83.txt");

            for (unsigned int j = 60000; j < 60100; j++) {
                if (j % 3 == 0) {
                    p1[j] = i;
                } else if (j % 3 == 1) {
                    p1[j] = j;
                } else {
                    p1[j] = pid1;
                }
                vm_yield();
                cout << "Child 1: " << p1[j] << endl;
            }
            return 0;
        }

        pid2 = fork();
        if (pid2 == 0) { // Child 2
            p2 = (char *) vm_map(nullptr, 0);
            strcpy(p2, "lampson83.txt");

            for (unsigned int j = 60100; j < 60200; j++) {
                if (j % 3 == 0) {
                    p2[j] = i;
                } else if (j % 3 == 1) {
                    p2[j] = j;
                } else {
                    p2[j] = pid2;
                }
                vm_yield();
                cout << "Child 2: " << p2[j] << endl;
            }
            return 0;
        }

        pid3 = fork();
        if (pid3 == 0) { // Child 3
            p3 = (char *) vm_map(nullptr, 0);
            strcpy(p3, "lampson83.txt");

            for (unsigned int j = 60200; j < 60300; j++) {
                if (j % 3 == 0) {
                    p3[j] = i;
                } else if (j % 3 == 1) {
                    p3[j] = j;
                } else {
                    p3[j] = pid3;
                }
                vm_yield();
                cout << "Child 3: " << p3[j] << endl;
            }
            return 0;
        }
    }
    return 0;
}
