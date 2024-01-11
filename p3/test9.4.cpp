#include "vm_app.h"
#include <iostream>
//caused error when using solution pager: pager produced too much output


int main() {
  const int NUM_PAGES = 80;

  // Map NUM_PAGES new virtual pages backed by swap files.
  void* pages[NUM_PAGES];
  for (int i = 0; i < NUM_PAGES; i++) {
    pages[i] = vm_map(nullptr, 0);
    if (pages[i] == nullptr) {
      // std::cerr << "Error: Failed to map page " << i << std::endl;
      return 1;
    }
  }

  // Write some data to the virtual pages.
  for (int i = 0; i < NUM_PAGES; i++) {
    char* data = (char*) pages[i];
    data[0] = 'A' + (i % 26); // Write a different letter to each page.
  }

  // Yield the CPU to another process.
  vm_yield();

  // Read the data from the virtual pages.
  for (int i = 0; i < NUM_PAGES; i++) {
    char* data = (char*) pages[i];
    // std::cout << data[0];
  }
  // std::cout << std::endl;

  return 0;
}
