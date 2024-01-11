/*
 * disk.h -- public interface to disk scheduler output functions.
 *
 */
#ifndef _DISK_H
#define _DISK_H

extern void print_request(unsigned int requester, unsigned int track);
extern void print_service(unsigned int requester, unsigned int track);
void manage_thread(void *a);
void server_thread(void* a);
void request_thread(void *a);
int main(int argc, char *argv[]);
#endif /* _DISK_H */
