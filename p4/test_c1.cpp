#include <iostream>
#include <cassert>
#include <cstdlib>
#include "fs_client.h"
#include <cstring>
#include <thread>

using std::cout;
using std::endl;



int main(int argc, char *argv[]) {
   char *server;
   int server_port;

   char arr[FS_BLOCKSIZE];
   char *writedata = "We these truths to be self-evident, that all men are created equal, that they are endowed by their Creator with certain unalienable Rights, that among these are Life, Liberty and the pursuit of Happiness. -- That to secure these rights, Governments are instituted among Men, deriving their just powers from the consent of the governed, -- That whenever any Form of Government becomes destructive of these ends, it is the Right of the People to alter or to abolish it, and to institute new Government, laying its foundation on such principles and organizing its powers in such form, as to them shall seem most likely to effect their Safety and Happiness.";
   // 
   memcpy(arr, writedata, FS_BLOCKSIZE);
   arr[5] = '\0';
   
   int status;

   if (argc != 3)
   {
   cout << "error: usage: " << argv[0] << " <server> <serverPort>\n";
   exit(1);
   }
   server = argv[1];
   server_port = atoi(argv[2]);

   fs_clientinit(server, server_port);
   // Issue fs_create, which will sleep for 10 seconds
   std::thread create_thread(fs_create, "user1", "/dir/slow", 'f');
   // Increase likelihood that file server receives fs_create first
   std::this_thread::sleep_for(std::chrono::seconds(3)); // or sleep(3)

   // Issue fs_readblock, which should not be blocked by first request
   char readdata[FS_BLOCKSIZE];
   std::thread read_thread(fs_readblock, "user1", "/file", 0, readdata);

   create_thread.join();
   read_thread.join();
}
