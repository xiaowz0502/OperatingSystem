#include <iostream>
#include <cassert>
#include <cstdlib>
#include "fs_client.h"
#include <cstring>

using std::cout;
using std::endl;

int main(int argc, char *argv[])
{
    char *server;
    int server_port;

    char arr[FS_BLOCKSIZE];
    char *writedata = "We these truths to be self-evident, that all men are created equal, that they are endowed by their Creator with certain unalienable Rights, that among these are Life, Liberty and the pursuit of Happiness. -- That to secure these rights, Governments are instituted among Men, deriving their just powers from the consent of the governed, -- That whenever any Form of Government becomes destructive of these ends, it is the Right of the People to alter or to abolish it, and to institute new Government, laying its foundation on such principles and organizing its powers in such form, as to them shall seem most likely to effect their Safety and Happiness.";
    // 
    memcpy(arr, writedata, FS_BLOCKSIZE);
    arr[5] = '\0';
    char readdata[FS_BLOCKSIZE];
    int status;

    if (argc != 3)
    {
        cout << "error: usage: " << argv[0] << " <server> <serverPort>\n";
        exit(1);
    }
    server = argv[1];
    server_port = atoi(argv[2]);

    fs_clientinit(server, server_port);

    status = fs_create("use /r", "", 'd');
    assert(status);

    status = fs_create("user1", "/dir", 'd');
    assert(!status);

    status = fs_writeblock("user1", "/dir", 00, writedata);
    assert(status);

    status = fs_readblock("user1", "/dir", 00, readdata);
    assert(status);

    status = fs_writeblock("user1", "/dir/file", 00, writedata);
    assert(status);

    status = fs_readblock("user1", "/dir/file", 00, readdata);
    assert(status);

    status = fs_create("user1", "/dir", 'f');
    assert(status);

    status = fs_create("user1", "/dir", 'd');
    assert(status);

    status = fs_delete("user1", "/dir");
    assert(!status);

    status = fs_create("user1", "/dir", 'f');
    assert(!status);

    status = fs_create("user1", "/dir", 'f');
    assert(status);

    status = fs_create("user1", "/dir", 'd');
    assert(status);

    status = fs_writeblock("user1", "/dir", 00, writedata);
    assert(!status);

    status = fs_readblock("user1", "/dir", 00, readdata);
    assert(!status);

    status = fs_writeblock("user1", "/dir/file", 00, writedata);
    assert(status);

    status = fs_readblock("user1", "/dir/file", 00, readdata);
    assert(status);
}
