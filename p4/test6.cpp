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
    fs_delete("user1", "/file");
    fs_readblock("user1", "/file", 0, writedata);
}
