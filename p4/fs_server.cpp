#include "fs_server.h"
#include "fs_param.h"
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
#include <unistd.h>
#include <regex>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <deque>
#include <mutex>
#include <utility>

using std::cout;
using std::deque;
using std::endl;
using std::mutex;
using std::regex;

deque<uint32_t> free_blocks;


uint32_t max_length = FS_MAXPATHNAME +FS_MAXUSERNAME + 20;

struct sockaddr_in addr;

deque<mutex *> locks;
mutex fb_lock;

class lock_guard
{
public:
    // acquire resources in constructor
    lock_guard(uint32_t in)
    {
        this->block_num = in;
        locks[block_num]->lock();
    }

    // swap
    void assign(uint32_t curr_block)
    {
        this->block_num = curr_block;
    }

    // release resources in destructor
    ~lock_guard()
    {
        locks[block_num]->unlock();
    }

private:
    uint32_t block_num;
};

uint32_t traverse(std::deque<std::string> *paths, std::deque<std::string> *words, lock_guard *outer, fs_inode *inode)
{

    int curr = 0;
    uint32_t curblock = 0;
    bool found = false;
    disk_readblock(0, inode);
    while (curr < paths->size() && inode->size > 0)
    {
        found = false;
        if (curblock != 0 && strcmp(inode->owner, (*words)[1].c_str()) != 0)
        {
            return 0;
        }

        for (int j = 0; j < inode->size; j++)
        {
            fs_direntry entries[FS_DIRENTRIES];
            disk_readblock(inode->blocks[j], &entries);

            for (int i = 0; i < FS_DIRENTRIES; i++)
            {
                if (entries[i].inode_block == static_cast<uint32_t>(0))
                {
                    continue;
                }

                std::string filename(entries[i].name);

                if (filename == (*paths)[curr])
                {
                    {
                        lock_guard inner(entries[i].inode_block);
                        inner.assign(curblock);
                        outer->assign(entries[i].inode_block);
                    }
                    
                    found = true;
                    curblock = entries[i].inode_block;

                    disk_readblock(entries[i].inode_block, inode);
                    if (curblock != 0 && strcmp(inode->owner, (*words)[1].c_str()) != 0)
                    {
                        return 0;
                    }
                    
                    if((curr < paths->size() - 1) && inode->type != 'd'){
                        return 0;
                    }
                    
                    if (curr == paths->size() - 1)
                    {
                        return entries[i].inode_block;
                    }
                    curr++;
                    break;
                }
            }
            if (found)
            {
                break;
            }
        }
        if (!found)
        {
            break;
        }
    }

    return 0;
}

bool error_checking(char act, std::deque<std::string> *words)
{
    if ((*words)[1].size() > FS_MAXUSERNAME ||(*words)[1].size() == 0 || (*words)[2].size() > FS_MAXPATHNAME || (*words)[2].size() == 0 || (*words)[2][0] != '/' || (*words)[2][(*words)[2].size() - 1] == '/')
    {
        if ((*words)[2] != "/")
        {
            return false;
        }
    }

    if (act == 'r' || act == 'w')
    {

        if ((*words)[3].size() > 9)
        {
            return false;
        }

        for (int i = 0; i < (*words)[3].size(); i++)
        {
            if (std::isdigit((*words)[3][i]) == 0)
            {
                return false;
            }
        }

        if((*words)[3].size() > 1 && (*words)[3][0] == '0'){
            return false;
        }

        if (std::stoi((*words)[3]) >= FS_MAXFILEBLOCKS)
        {
            return false;
        }


    }

    else if (act == 'c')
    {
        fb_lock.lock();
        if (((*words)[3] != "f" && (*words)[3] != "d") || free_blocks.empty())
        {
            fb_lock.unlock();
            return false;
        }
        fb_lock.unlock();

        if ((*words)[2] == "/")
        {
            return false;
        }
    }

    else if (act == 'd')
    {
        if ((*words)[2] == "/")
        {
            return false;
        }
    }

    return true;
}

void send_bytes(int sock, const char *buf, int len)
{
    size_t sent = 0;
    do
    {
        sent += send(sock, buf + sent, len - sent, MSG_NOSIGNAL);
    } while (sent < len);
}

void read_block(int conn, std::deque<std::string> *words, std::deque<std::string> *paths)
{
    {
        lock_guard outer(0);
        fs_inode inode;
        uint32_t block = traverse(paths, words, &outer, &inode);
        if (block != 0)
        {

            if (inode.type != 'f' || std::stoi((*words)[3]) >= inode.size)
            {
                close(conn);
                return;
            }

            char data[FS_BLOCKSIZE];
            disk_readblock(inode.blocks[std::stoi((*words)[3])], &data);
            std::string temp = (*words)[0] + " " + (*words)[1] + " " + (*words)[2] + " " + (*words)[3];

            char output[temp.size() + FS_BLOCKSIZE + 1];
            for (unsigned int i = 0; i < temp.size(); i++)
            {
                output[i] = temp[i];
            }

            output[temp.size()] = '\0';

            for (unsigned int i = 0; i < FS_BLOCKSIZE; i++)
            {
                output[i + temp.size() + 1] = data[i];
            }

            send_bytes(conn, output, temp.size() + FS_BLOCKSIZE + 1);
        }
    }
    close(conn);
}


void write_block(int conn, std::deque<std::string> *words, const char *data, std::deque<std::string> *paths)
{
    {
        lock_guard outer(0);
        fs_inode inode;
        uint32_t target = traverse(paths, words, &outer, &inode);

        if (target != 0)
        {
            if (inode.type != 'f' || std::stoi((*words)[3]) > inode.size)
            {
                close(conn);
                return;
            }

            uint32_t free_b;
            bool overwrite = false;
            if (std::stoi((*words)[3]) == inode.size)
            {   
                fb_lock.lock();
                if (inode.size == FS_MAXFILEBLOCKS || free_blocks.empty())
                {
                    fb_lock.unlock();
                    close(conn);
                    return;
                }

                free_b = free_blocks.front();
                free_blocks.pop_front();
                fb_lock.unlock();

                inode.size++;
                overwrite = false;
            }
            else{
                free_b = inode.blocks[std::stoi((*words)[3])];
                overwrite = true;
            }
            

            char data_block[FS_BLOCKSIZE];
            for (int i = 0; i < FS_BLOCKSIZE; i++){
                data_block[i] = data[i];
            }

            disk_writeblock(free_b, data_block);

            if (overwrite == false){
                inode.blocks[std::stoi((*words)[3])] = free_b;
                disk_writeblock(target, &inode);
            }

            std::string temp = (*words)[0] + " " + (*words)[1] + " " + (*words)[2] + " " + (*words)[3] + '\0';
            const char *output = temp.c_str();

            // fb_lock.lock();
            // free_blocks.pop_front();
            // fb_lock.unlock();

            send_bytes(conn, output, temp.length());
        }
    }
    close(conn);
}


void create_block(int conn, std::deque<std::string> *words, std::deque<std::string> *paths)
{
    {
        lock_guard outer(0);
        std::string file = paths->back(); // ready to be created
        paths->pop_back();

        fs_inode inode;
        uint32_t block = traverse(paths, words, &outer, &inode);

        if (block != 0 || paths->size() == 0)
        {

            if (inode.type != 'd')
            {
                close(conn);
                return;
            }

            bool find_lowest = false;
            uint32_t lowest;
            size_t index;

            fs_direntry lowest_entry[FS_DIRENTRIES];

            // check for duplicate
            for (int i = 0; i < inode.size; i++)
            {
                fs_direntry entry[FS_DIRENTRIES];
                disk_readblock(inode.blocks[i], &entry);

                for (int j = 0; j < FS_DIRENTRIES; j++)
                {
                    if (entry[j].inode_block == static_cast<u_int32_t>(0))
                    {
                        // find lowest available location
                        if (find_lowest == false)
                        {
                            memcpy(lowest_entry, entry, FS_BLOCKSIZE);
                            index = j;
                            lowest = inode.blocks[i];
                            find_lowest = true;
                        }
                        continue;
                    }
                    if (strcmp(entry[j].name, file.c_str()) == 0)
                    {
                        close(conn);
                        return;
                    }
                }
            }

            if (find_lowest == false && inode.size == FS_MAXFILEBLOCKS)
            {
                close(conn);
                return;
            }

            uint32_t first; // for inode
            uint32_t second; // for new direntry block

            fb_lock.lock();
            if (find_lowest == false && free_blocks.size() < 2) 
            {
                fb_lock.unlock();
                close(conn);
                return;
            }
            else if(find_lowest == false && free_blocks.size() >= 2){
                first = free_blocks.front();
                free_blocks.pop_front();
                second = free_blocks.front();
                free_blocks.pop_front();
            }
            else if(find_lowest == true && free_blocks.size() >= 1){
                first = free_blocks.front();
                free_blocks.pop_front();
            }
            else{
                fb_lock.unlock();
                close(conn);
                return;
            }
            
            fb_lock.unlock();

            fs_inode new_inode;
            new_inode.type = (*words)[3][0];
            new_inode.size = 0;
            strcpy(new_inode.owner, (*words)[1].c_str());


            disk_writeblock(first, &new_inode);

            fs_direntry entries[FS_DIRENTRIES];
            if (find_lowest == false)
            {
                for (size_t i = 0; i < FS_DIRENTRIES; i++)
                {
                    if (i == 0)
                    {
                        strcpy(entries[i].name, file.c_str());
                        entries[i].inode_block = first;
                    }
                    else
                    {
                        entries[i].inode_block = 0;
                    }
                }
                
                disk_writeblock(second, &entries);


                inode.size++;
                inode.blocks[inode.size - 1] = second;


                disk_writeblock(block, &inode);
            }
            else
            {
                strcpy(lowest_entry[index].name, file.c_str());
                lowest_entry[index].inode_block = first;
                disk_writeblock(lowest, &lowest_entry);
            }


            std::string temp = (*words)[0] + " " + (*words)[1] + " " + (*words)[2] + " " + (*words)[3] + '\0';
            const char *output = temp.c_str();
            send_bytes(conn, output, temp.length());
        }
    }
    close(conn);
}

void delete_block(int conn, std::deque<std::string> *words, std::deque<std::string> *paths)
{
    {
        lock_guard outer(0);
        std::string file = paths->back(); 
        paths->pop_back();

        fs_inode inode;
        uint32_t block = traverse(paths, words, &outer, &inode);
        uint32_t ready_delete;

        if (block != 0 || paths->size() == 0)
        {

            if (inode.type != 'd')
            {
                close(conn);
                return;
            }

            bool found = false;
            uint32_t dir_block;
            size_t i_index;
            size_t d_index;
            fs_direntry entry[FS_DIRENTRIES];
            fs_direntry delete_entry[FS_DIRENTRIES];

            // find the file/directory
            for (size_t i = 0; i < inode.size; i++)
            {
                disk_readblock(inode.blocks[i], &entry);

                for (size_t j = 0; j < FS_DIRENTRIES; j++)
                {
                    if (entry[j].inode_block == static_cast<u_int32_t>(0))
                    {
                        continue;
                    }
                    if (strcmp(entry[j].name, file.c_str()) == 0)
                    {
                        ready_delete = entry[j].inode_block;
                        memcpy(delete_entry, entry, FS_BLOCKSIZE);
                        dir_block = inode.blocks[i];
                        i_index = i;
                        d_index = j;
                        found = true;
                        break;
                    }
                }
                if (found)
                {
                    break;
                }
            }

            if (!found)
            {
                close(conn);
                return;
            }

            

            lock_guard r_d(ready_delete);
            fs_inode node; 
            disk_readblock(ready_delete, &node);

            if (node.type == 'd' && node.size != 0)
            {
                close(conn);
                return;
            }

            if (strcmp(node.owner, (*words)[1].c_str()) != 0)
            {
                close(conn);
                return;
            }
            
            int count = 0;
            
            for (size_t i = 0; i < FS_DIRENTRIES; i++)
            {
                if (delete_entry[i].inode_block != static_cast<uint32_t>(0))
                {
                    count++;
                }
            }

            if (count > 1)
            {
                delete_entry[d_index].inode_block = 0;
                disk_writeblock(dir_block, &delete_entry);
            }
            else if (count == 1)
            {

                for (uint32_t i = i_index; i < inode.size - 1; i++)
                {
                    inode.blocks[i] = inode.blocks[i + 1];
                }
                inode.size = inode.size - 1;
                disk_writeblock(block, &inode);

                fb_lock.lock();
                free_blocks.push_back(dir_block);
                
                fb_lock.unlock();

            }
            
            if (node.size != 0)
            {
                
                fb_lock.lock();

                free_blocks.push_back(node.blocks[0]);
                

                fb_lock.unlock();
                for (size_t i = 1; i < node.size; i++)
                {
                
                    fb_lock.lock();
                    free_blocks.push_back(node.blocks[i]);
                    
                    fb_lock.unlock();
                }
            }
            fb_lock.lock();
            free_blocks.push_back(ready_delete);

            fb_lock.unlock();

            std::string temp = (*words)[0] + " " + (*words)[1] + " " + (*words)[2] + '\0';
            const char *output = temp.c_str();
            send_bytes(conn, output, temp.length());
        }
    }
    close(conn);
}

void initialization(uint32_t block, char type)
{
    if (type == 'i')
    {
        fs_inode inode;
        disk_readblock(block, &inode);
        if (inode.type == 'd')
        {
            for (size_t i = 0; i < inode.size; i++)
            {
                initialization(inode.blocks[i], 'd');
            }
        }
    }
    else if (type == 'd')
    {
        fs_direntry entries[FS_DIRENTRIES];
        disk_readblock(block, &entries);
        for (size_t i = 0; i < FS_DIRENTRIES; i++)
        {
            if (entries[i].inode_block == static_cast<u_int32_t>(0))
            {
                continue;
            }
            initialization(entries[i].inode_block, 'i');
        }
    }
    fb_lock.lock();
    auto it = find(free_blocks.begin(), free_blocks.end(), block);
    if (it != free_blocks.end())
    {
        free_blocks.erase(it);
    }

    fb_lock.unlock();
}

void wrapper(int conn)
{

    std::string message;
    char buf[1];
    do
    {
        int bytes_recvd = recv(conn, buf, sizeof(buf), 0);
        if (bytes_recvd <= 0)
        {

            close(conn);
            return;
        }

        if (buf[0] == '\0')
        {
            break;
        }

        message += std::string(buf, bytes_recvd);

        if (message.size() >= max_length){
            break;
        }

    } while (buf[0] != '\0');

    if (message.size() > max_length){
        close(conn);
        return;
    }
    
    std::istringstream ss(message);
    std::string word;
    std::deque<std::string> words;
    while (ss >> word)
    {
        words.push_back(word);
    }

    bool no_error = false;


    if (words[0] == "FS_READBLOCK")
    {
        if (words.size() != 4){
            close(conn);
            return;
        }
        std::string temp = words[0] + " " + words[1] + " " + words[2] + " " + words[3];
        if (error_checking('r', &words) && (temp == message)){
            no_error = true;
        }
    }
    else if (words[0] == "FS_WRITEBLOCK")
    {
        if (words.size() != 4){
            close(conn);
            return;
        }
        std::string temp = words[0] + " " + words[1] + " " + words[2] + " " + words[3];
        if (error_checking('w', &words) && (temp == message)){
            no_error = true;
        }
    }
    else if (words[0] == "FS_CREATE")
    {
        if (words.size() != 4){
            close(conn);
            return;
        }
        std::string temp = words[0] + " " + words[1] + " " + words[2] + " " + words[3];
        if (error_checking('c', &words) && (temp == message)){
            no_error = true;
        }
    }

    else if (words[0] == "FS_DELETE")
    {   
        if (words.size() != 3){
            close(conn);
            return;
        }
        std::string temp = words[0] + " " + words[1] + " " + words[2];
        if (error_checking('d', &words) && (temp == message)){
            no_error = true;
        }
    }

    if (!no_error)
    {
        close(conn);
        return;
    }


    std::istringstream ss2(words[2].substr(1));
    std::string temp;
    std::deque<std::string> paths;
    while (std::getline(ss2, temp, '/'))
    {
        paths.push_back(temp);
    }


    if (paths.size() > 0)
    {
        for (int i = 0; i < paths.size(); i++)
        {
            if (paths[i] == "" || paths[i].length() > FS_MAXFILENAME)
            {
                close(conn);
                return;
            }
        }
    }


    if (words[0] == "FS_READBLOCK")
    {
        read_block(conn, &words, &paths);
    }
    else if (words[0] == "FS_WRITEBLOCK")
    {
        char buff[FS_BLOCKSIZE];
        if (recv(conn, buff, sizeof(buff), MSG_WAITALL) <= 0)
        {
            close(conn);
            return;
        }

        write_block(conn, &words, buff, &paths);
    }
    else if (words[0] == "FS_CREATE")
    {
        create_block(conn, &words, &paths);
    }

    else if (words[0] == "FS_DELETE")
    {
        delete_block(conn, &words, &paths);
    }

}

int get_port_number(int sockfd)
{

    socklen_t length = sizeof(addr);
    if (getsockname(sockfd, (sockaddr *)&addr, &length) == -1)
    {
        perror("Error getting port of socket");
        return -1;
    }

    return ntohs(addr.sin_port);
}

int main(int argc, char *argv[])
{
    int port_number = 0;

    fb_lock.lock();
    for (uint32_t i = 0; i < FS_DISKSIZE; i++)
    {
        free_blocks.push_back(i);
        mutex *m = new mutex();
        locks.push_back(m);
    }
    fb_lock.unlock();

    initialization(0, 'i');

    if (argc == 2)
    {
        port_number = atoi(argv[1]);
        cout << "port number is " << argv[1] << endl;
    }
    else
    {
        port_number = 0;
    }


    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Error opening socket");
        exit(1);
    }


    int yesval = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yesval, sizeof(yesval)) == -1)
    {
        perror("Error setting socket options");
        return -1;
    }


    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port_number);

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("Error binding socket");
        exit(1);
    }

    port_number = get_port_number(sockfd);
    cout_lock.lock();
    std::cout << "\n@@@ port " << ntohs(addr.sin_port) << std::endl;
    cout_lock.unlock();

    listen(sockfd, 30);
    while (true)
    {
        int conn = accept(sockfd, 0, 0);
        if (conn < 0)
        {
            perror("Error accepting connection");
            continue;
        }

        std::thread t1 = std::thread(wrapper, conn);
        t1.detach();
    }

    while (!locks.empty())
    {
        delete locks.back();
        locks.pop_back();
    }
}
