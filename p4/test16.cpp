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

    int status;
    server = argv[1];
    server_port = atoi(argv[2]);
    const char *writedata = "We hold these truths to be self-evident, that all men are created equal, that they are endowed by their Creator with certain unalienable Rights, that among these are Life, Liberty and the pursuit of Happiness. -- That to secure these rights, Governments are instituted among Men, deriving their just powers from the consent of the governed, -- That whenever any Form of Government becomes destructive of these ends, it is the Right of the People to alter or to abolish it, and to institute new Government, laying its foundation on such principles and organizing its powers in such form, as to them shall seem most likely to effect their Safety and Happiness.";


    fs_clientinit(server, server_port);

    status = fs_delete("user1", "/filename1/2");
    assert(!status);

    status = fs_delete("user1", "/filename1/1");
    assert(!status);

    status = fs_create("user1", "/filename1/1", 'f');
    assert(!status);

    status = fs_writeblock("user1", "/filename1/1", '124', writedata);
    assert(status);
    
    status = fs_writeblock("user1", "/filename1/1", '0', writedata);
    assert(status);

    status = fs_create("user1", "/filename1/1", 'f');
    assert(status);

    
}