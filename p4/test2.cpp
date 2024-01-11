#include <iostream>
#include <cassert>
#include <cstdlib>
#include "fs_client.h"
#include <thread>
#include <cstring>

using std::cout;

void thread_wait()
{
    std::this_thread::sleep_for(std::chrono::seconds(10));
    fs_create("user1", "/dir/slow", 'f');
}

int main(int argc, char *argv[])
{
    char *server;
    int server_port;
    int status;

    server = argv[1];
    server_port = atoi(argv[2]);

    fs_clientinit(server, server_port);

    for (int i = 0; i < 12; i++) {
        std::string filename = "/filename" + std::to_string(i);
        status = fs_create("user1", filename.c_str(), 'd');
        for (int j = 0; j < 12; j++) {
            filename += "/" + std::to_string(j);
            status = fs_create("user1", filename.c_str(), 'f');
        }
    }

    const char *writedata = "We hold these truths to be self-evident, that all men are created equal, that they are endowed by their Creator with certain unalienable Rights, that among these are Life, Liberty and the pursuit of Happiness. -- That to secure these rights, Governments are instituted among Men, deriving their just powers from the consent of the governed, -- That whenever any Form of Government becomes destructive of these ends, it is the Right of the People to alter or to abolish it, and to institute new Government, laying its foundation on such principles and organizing its powers in such form, as to them shall seem most likely to effect their Safety and Happiness.";

    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 12; j++) {
            std::string filename = "/filename" + std::to_string(i) + "/" + std::to_string(j);
            status = fs_writeblock("user1", filename.c_str(), 0, writedata);
        }
    }

    char readdata[FS_BLOCKSIZE];
    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 12; j++) {
            std::string filename = "/filename" + std::to_string(i) + "/" + std::to_string(j);
            status = fs_readblock("user1", filename.c_str(), 0, readdata);
        }
    }

    for (int i = 0; i < 12; i++) {
        std::string filename = "/filename" + std::to_string(i);
        status = fs_delete("user1", filename.c_str());
        for (int j = 0; j < 12; j++) {
            filename += "/" + std::to_string(j);
            status = fs_delete("user1", filename.c_str());
        }
    }

    for (int i = 0; i < 10; i++) {
        std::string filename = "/filename" + std::to_string(i);
        status = fs_create("user1", filename.c_str(), 'd');
        for (int j = 0; j < 10; j++) {
            std::string filename = "/filename" + std::to_string(i) + "/" + std::to_string(j);
            status = fs_create("user1", filename.c_str(), 'f');
        }
    }

    for (int i = 0; i < 10; i++) {
        std::string filename = "/filename" + std::to_string(i);
        status = fs_delete("user1", filename.c_str());
        for (int j = 0; j < 10; j++) {
            std::string filename = "/filename" + std::to_string(i) + "/" + std::to_string(j);
            status = fs_delete("user1", filename.c_str());
        }
    }
}
