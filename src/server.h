#ifndef SERVER_H
#define SERVER_H

#include "hashmap.h"
#include "eventloop.h"
#include "db.h"
#include <netinet/in.h>
#include "connection.h"

#define MAX_CONN_FD 10240

struct server_config
{
    int port;
    int max_conns;
};

struct server 
{
    int fd;
    struct server_config config;
    struct eventloop *eventloop;
    struct connection *conns[MAX_CONN_FD];
    struct database *db;
    struct server_op {
        int (*start)(struct server *self);
        void (*shutdown)(struct server *self);
        void (*event_handler)(struct server *self);
        int (*accept)(struct server *self, struct event ev);
        void (*handle_conn_read)(struct server *self, struct connection *conn);
        void (*handle_conn_write)(struct server *self, struct connection *conn);
        void (*handle_conn_hup)(struct server *self, struct connection *conn);
    } op;
};

struct server *create_server(struct server_config config);

#endif