// delete shifting case

/*
The client sends an FS_READBLOCK request to read a non-existing file or a non-existing block of an existing file.
The client sends an FS_WRITEBLOCK request to write data to a non-existing file.
The client sends an FS_CREATE request to create a file or directory that already exists. 写不了
The client sends an FS_CREATE request with an invalid type (not 'f' or 'd').
The client sends an FS_DELETE request to delete a non-existing file or directory. 写不了
The client sends an FS_DELETE request to delete a non-empty directory. 真的有必要查么
*/

#include <iostream>
#include <cassert>
#include <cstdlib>
#include "fs_client.h"

using std::cout;

int main(int argc, char *argv[])
{
    char *server;
    int server_port;

    const char *writedata = "We hold these truths to be self-evident, that all men are created equal, that they are endowed by their Creator with certain unalienable Rights, that among these are Life, Liberty and the pursuit of Happiness. -- That to secure these rights, Governments are instituted among Men, deriving their just powers from the consent of the governed, -- That whenever any Form of Government becomes destructive of these ends, it is the Right of the People to alter or to abolish it, and to institute new Government, laying its foundation on such principles and organizing its powers in such form, as to them shall seem most likely to effect their Safety and Happiness.";

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

    status = fs_create("user1", "/dir", 'd');
    status = fs_create("user1", "/dir/1", 'f');
    status = fs_create("user1", "/dir/2", 'f');
    status = fs_delete("user1", "/dir/3");
    status = fs_create("user1", "/dir/4", 'f');
    status = fs_create("user1", "/dir/5", 'd');
    status = fs_create("user1", "/dir/6", 'f');
    status = fs_delete("user1", "/dir/7");
    status = fs_create("user1", "/dir/8", 'f');
    status = fs_create("user1", "/dir/9", 'd');
    status = fs_delete("user1", "/dir/1");
    status = fs_create("user1", "/dir/10", 'f');
    status = fs_delete("user1", "/dir");
}
