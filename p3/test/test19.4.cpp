#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

int main() {
    const int NUM_PROCESSES = 4;
    const int NUM_PAGES = 2;
    const int PAGE_SIZE = 4096;

    for (int i = 0; i < NUM_PROCESSES; i++) {
        int pid = fork();
        char* mem = (char*) vm_map(nullptr, NUM_PAGES);

        for (int j = 0; j < NUM_PAGES * PAGE_SIZE; j++) {
            mem[j] = i + '0';
        }

        vm_yield();
    }

    char* mem = (char*) vm_map(nullptr, NUM_PAGES * NUM_PROCESSES);

    for (int i = 0; i < NUM_PROCESSES; i++) {
        for (int j = 0; j < NUM_PAGES * PAGE_SIZE; j++) {
            std::cout << mem[i * NUM_PAGES * PAGE_SIZE + j];
        }
        std::cout << std::endl;
    }

    return 0;
}