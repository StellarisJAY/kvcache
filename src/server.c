#include "server.h"
#include "eventloop.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

int server_start(struct server *s)
{
    if (listen(s->fd, 128) < 0) {
        printf("server listen error: %s\n", strerror(errno));
        eventloop_free(s->eventloop);
        return -1;
    }
    printf("server started, port: %d\n", s->config.port);
    s->op.event_handler(s);
}

void server_shutdown(struct server *s)
{
    // todo shutdown server
}

void server_event_handler(struct server *s)
{
    while(1) {
        int n = eventloop_wait(s->eventloop);
        if (n < 0) {
            break;
        }
        for (int i = 0; i < n; i++) {
            struct event ev = s->eventloop->ready_events[i];
            if (ev.fd == s->fd) {
                s->op.accept(s, ev);
                continue;
            }
            struct connection *conn = s->conns[ev.fd];
            if (conn ==NULL) continue;
            if (ev.mask & EVENT_READ) s->op.handle_conn_read(s, conn);
            if (ev.mask & EVENT_WRITE) s->op.handle_conn_write(s, conn);
        }
    }
}

int server_accept(struct server *s, struct event ev)
{
    struct connection *conn = malloc(sizeof(struct connection));
    int addr_len = sizeof(struct sockaddr_in);
    int fd = accept(s->fd, (struct sockaddr *)&conn->addr, &addr_len);
    if (fd < 0) {
        printf("accept conn error: %s\n", strerror(errno));
        free(conn);
        return -1;
    }
    if (eventloop_add(s->eventloop, fd, EVENT_READ | EVENT_WRITE | EVENT_HUP | EVENT_ERR) < 0) {
        free(conn);
        return -1;
    }
    conn->fd = fd;
    s->conns[fd] = conn;
    return 0;
}

void server_handle_conn_read(struct server *s, struct connection *conn)
{
    char buf[256];
    while(1) {
        int n = recv(conn->fd, buf, 256, MSG_DONTWAIT);
        if (n == -1) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) break;
            else return;
        }
    }
}

void server_handle_conn_write(struct server *s, struct connection *conn)
{
    char *buf = "HTTP/1.1 200 OK\r\n123\r\n";
    int n = send(conn->fd, buf, strlen(buf), MSG_DONTWAIT);
    if (n < 0) {
        printf("write resp error: %s\n", strerror(errno));
        return;
    }
    eventloop_delete(s->eventloop, conn->fd);
    shutdown(conn->fd, SHUT_RDWR);
}

struct server *create_server(struct server_config config)
{
    struct server *s = malloc(sizeof(struct server));
    s->config = config;
    s->eventloop = create_eventloop(config.max_conns);
    
    struct sockaddr_in addr;
    addr.sin_port = htons(config.port);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htons(INADDR_ANY);

    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        printf("create socket error: %s\n", strerror(errno));
        eventloop_free(s->eventloop);
        free(s);
        return NULL;
    }
    if (eventloop_add(s->eventloop, sock_fd, EVENT_READ) < 0) {
        eventloop_free(s->eventloop);
        free(s);
        return NULL;
    }

    int addr_len = sizeof(struct sockaddr);
    if (bind(sock_fd, (struct sockaddr *)&addr, addr_len) < 0) {
        printf("bind error: %s\n", strerror(errno));
        eventloop_free(s->eventloop);
        free(s);
        return NULL;
    }

    s->fd = sock_fd;
    struct server_op op = {
        .start = server_start,
        .event_handler = server_event_handler,
        .accept = server_accept,
        .shutdown = server_shutdown,
        .handle_conn_read = server_handle_conn_read,
        .handle_conn_write = server_handle_conn_write,
    };
    s->op = op;
    return s;
}