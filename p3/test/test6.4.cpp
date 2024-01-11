#include "vm_app.h"
#include <iostream>

int main() {
  // Map a new virtual page backed by swap file.
  void *page1 = vm_map(nullptr, 0);
  if (page1 == nullptr) {
    std::cerr << "Error: Failed to map page1" << std::endl;
    return 1;
  }
  
  // Write some data to the new virtual page.
  char *data1 = (char*) page1;
  data1[0] = 'H';
  data1[1] = 'e';
  data1[2] = 'l';
  data1[3] = 'l';
  data1[4] = 'o';
  data1[5] = '\0';

  // Yield the CPU to another process.
  vm_yield();

  // Map another virtual page backed by a file.
  void *page2 = vm_map("lampson83.txt", 0);
  if (page2 == nullptr) {
    std::cerr << "Error: Failed to map page2" << std::endl;
    return 1;
  }

  // Write some data to the new virtual page.
  char *data2 = (char*) page2;
  data2[0] = 'W';
  data2[1] = 'o';
  data2[2] = 'r';
  data2[3] = 'l';
  data2[4] = 'd';
  data2[5] = '\0';

  // Yield the CPU to another process.
  vm_yield();

  // Read the data from the first virtual page.
  std::cout << data1 << std::endl;

  // Yield the CPU to another process.
  vm_yield();

  // Read the data from the second virtual page.
  std::cout << data2 << std::endl;

  return 0;
}