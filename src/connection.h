#ifndef CONNECTION_H
#define CONNECTION_H

#include <netinet/in.h>
struct connection
{
    int fd;
    struct sockaddr_in addr;
    char *write_buf;
    unsigned int buf_len;
    int selected_db;
};
#endif