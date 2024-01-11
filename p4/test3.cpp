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

    fs_create("user1", "/dir", 'd');
    fs_create("user1", "/dir/file", 'f');
    char writedata[FS_BLOCKSIZE];
    memset(writedata, '\0', FS_BLOCKSIZE);
    fs_writeblock("user1", "/dir/file", 0, writedata);

    std::this_thread::sleep_for(std::chrono::seconds(10));

    // Issue fs_create, which will sleep for 10 seconds
    std::thread create_thread(fs_create, "user1", "/dir/slow", 'f');
    // Increase likelihood that file server receives fs_create first

    std::this_thread::sleep_for(std::chrono::seconds(10)); // or sleep(3)

    // Issue fs_readblock, which should not be blocked by first request
    char readdata[FS_BLOCKSIZE];
    std::thread read_thread(fs_readblock, "user1", "/dir/file", 0, readdata);

    create_thread.join();
    read_thread.join();
}
