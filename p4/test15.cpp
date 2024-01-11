#include <iostream>
#include <cassert>
#include <cstdlib>
#include "fs_client.h"
#include <thread>
#include <cstring>

using std::cout;

int main(int argc, char *argv[])
{
    char *server;
    int server_port;

    server = argv[1];
    server_port = atoi(argv[2]);

    fs_clientinit(server, server_port);

    fs_create("user1", "/file", 'f');
    fs_create("user1", "/dir", 'd');
    char writedata[FS_BLOCKSIZE];
    memset(writedata, '\0', FS_BLOCKSIZE);
    fs_writeblock("user1", "/file", 0, writedata);

    // Issue fs_create,
    // which will sleep for 10 seconds
    std::thread delete_thread(fs_delete, "user1", "/dir");
    // Increase likelihood that file server receives fs_create first
    std::this_thread::sleep_for(std::chrono::seconds(3)); // or sleep(3)

    char readdata[FS_BLOCKSIZE];

    std::thread create_thread(fs_create, "user1", "/dir/slow", 'f');
    std::this_thread::sleep_for(std::chrono::seconds(3)); // or sleep(3)

    std::thread create_thread2(fs_readblock, "user1", "/dir/slow", 0, readdata);
    std::this_thread::sleep_for(std::chrono::seconds(3)); // or sleep(3)

    std::thread create_thread3(fs_writeblock, "user1", "/dir/slow", 0, writedata);
    std::this_thread::sleep_for(std::chrono::seconds(3)); // or sleep(3)

    std::thread create_thread4(fs_delete, "user1", "/dir/slow");

    create_thread.join();
    delete_thread.join();
    create_thread2.join();
    create_thread3.join();
    create_thread4.join();
}