#include "vm_app.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main() {
    // Test case 1
    char *filename = (char *) vm_map(nullptr, 0);
    strcpy(filename, "lampson83.txt");

    if (fork()) { // parent
        char *fb_page = (char *) vm_map(filename, 0);
        fb_page[0] = 'B';
        vm_yield();
        assert(fb_page[0] == 'H');
    } else { // child
        char *fb_page = (char *) vm_map(filename, 0);
        fb_page[0] = 'H';
    }

    // Test case 2
    void *page1 = vm_map(nullptr, 0);
    void *page2 = vm_map(nullptr, 0);
    void *page3 = vm_map(nullptr, 0);
    void *page4 = vm_map(nullptr, 0);
    void *page5 = vm_map(nullptr, 0);
    void *page6 = vm_map(nullptr, 0);

    assert(page1 != nullptr);
    assert(page2 != nullptr);
    assert(page3 != nullptr);
    assert(page4 != nullptr);
    assert(page5 != nullptr);
    assert(page6 == nullptr);

    // Test case 3
    char *invalid_filename = (char *) vm_map(nullptr, 0);
    strcpy(invalid_filename, "/invalid/file/path");

    void *invalid_page = vm_map(invalid_filename, 0);

    assert(invalid_page == nullptr);

    printf("All tests passed.\n");
    return 0;
}
