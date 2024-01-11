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

    for (size_t i = 0; i < 124; i++) {
        std::string s = "/file" + std::to_string(i);
        fs_create("user1", s.c_str(), 'f');
    }
    fs_create("user1", "/file124", 'f');
    fs_create("user1", "newfile", 'f');

    const char *writedata = "We hold these truths to be self-evident, that all men are created equal, that they are endowed by their Creator with certain unalienable Rights, that among these are Life, Liberty and the pursuit of Happiness. -- That to secure these rights, Governments are instituted among Men, deriving their just powers from the consent of the governed, -- That whenever any Form of Government becomes destructive of these ends, it is the Right of the People to alter or to abolish it, and to institute new Government, laying its foundation on such principles and organizing its powers in such form, as to them shall seem most likely to effect their Safety and Happiness.";


    for (size_t i = 0; i < 124; i++) {
        fs_writeblock("user1", "/file1", i, writedata);
    }
    fs_writeblock("user1", "/file1", 124, writedata);
}
