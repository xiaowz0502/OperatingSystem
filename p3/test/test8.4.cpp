#include "vm_app.h"
#include <iostream>

int main() {
  // Map five new virtual pages backed by swap files.
  void *page1 = vm_map(nullptr, 0);
  if (page1 == nullptr) {
    std::cerr << "Error: Failed to map page1" << std::endl;
    return 1;
  }
  void *page2 = vm_map(nullptr, 0);
  if (page2 == nullptr) {
    std::cerr << "Error: Failed to map page2" << std::endl;
    return 1;
  }
  void *page3 = vm_map(nullptr, 0);
  if (page3 == nullptr) {
    std::cerr << "Error: Failed to map page3" << std::endl;
    return 1;
  }
  void *page4 = vm_map(nullptr, 0);
  if (page4 == nullptr) {
    std::cerr << "Error: Failed to map page4" << std::endl;
    return 1;
  }
  void *page5 = vm_map(nullptr, 0);
  if (page5 == nullptr) {
    std::cerr << "Error: Failed to map page5" << std::endl;
    return 1;
  }

  // Write some data to the virtual pages.
  char *data1 = (char*) page1;
  char *data2 = (char*) page2;
  char *data3 = (char*) page3;
  char *data4 = (char*) page4;
  char *data5 = (char*) page5;
  data1[0] = 'H';
  data2[0] = 'E';
  data3[0] = 'L';
  data4[0] = 'L';
  data5[0] = 'O';

  // Yield the CPU to another process.
  vm_yield();

  // Read the data from the virtual pages.
  std::cout << data1[0] << std::endl;
  std::cout << data2[0] << std::endl;
  std::cout << data3[0] << std::endl;
  std::cout << data4[0] << std::endl;
  std::cout << data5[0] << std::endl;

  return 0;
}
