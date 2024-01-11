#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using namespace std;

int main() {
    int pid;
    char *p;
    
    for (int i = 0; i < 5; i++) {
        pid = fork();
        if (pid == 0) {
            // Child process
            p = (char *) vm_map(nullptr, 0);
            for (unsigned int j = 0; j < 100; j++) {
                if (j % 3 == 0) {
                    p[j] = i;
                } else if (j % 3 == 1) {
                    p[j] = j;
                } else {
                    p[j] = pid;
                }
                vm_yield();
            }
            cout << "Child process " << i << " done!" << endl;
            return 0;
        }
    }

    // Parent process
    for (int i = 0; i < 5; i++) {
        for(int j = 0; j < 65536; j++){
            cout << "Child process " << p[i] << " terminated." << endl;
        }
        
    }

    return 0;
}