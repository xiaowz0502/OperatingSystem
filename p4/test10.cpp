#include <iostream>
#include <cassert>
#include <cstdlib>
#include "fs_client.h"
#include <thread>
#include <cstring>

using std::cout;

int main(int argc, char *argv[]) {
    char *server;
    int server_port;

    server = argv[1];
    server_port = atoi(argv[2]);

    fs_clientinit(server, server_port);

    fs_create("user1", "/file", 'f');
    fs_create("user1", "/dir", 'd');
    fs_create("user1", "/dir1", 'd');
    fs_create("user1", "/dir2", 'd');
    fs_create("user1", "/dir3", 'd');
    fs_create("user1", "/dir4", 'd');
    fs_create("user1", "/dir5", 'd');
    fs_create("user1", "/dir6", 'd');
    fs_create("user1", "/dir7", 'd');
    char writedata[FS_BLOCKSIZE];
    memset(writedata, '\0', FS_BLOCKSIZE);
    fs_writeblock("user1", "/file", 0, writedata);
    fs_delete("user1", "/dir5");
    fs_create("user1", "/dir8", 'd');
}
