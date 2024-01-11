#include <iostream>
#include <cassert>
#include <cstdlib>
#include "fs_client.h"
#include <string>

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
    // excedd block array, 124*8 = 992
    for (int i = 0; i < 992; i++)
    {
        std::string filename = "/filename" + std::to_string(i);
        status = fs_create("user1", filename.c_str(), 'd');
    }

    // left with 3102 block
    status = fs_create("user1", "extra", 'd');

    for (int i = 0; i < 32; i++)
    {
        for (int j = 0; j < 992; j++)
        {
            std::string filename = "/filename" + std::to_string(j) + "/" + std::to_string(i);
            status = fs_create("user1", filename.c_str(), 'f');
        }
    }

    status = fs_create("user1", "/extra", 'd');
}
