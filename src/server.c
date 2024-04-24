#include "server.h"
#include "eventloop.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "protocol.h"

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
            if (ev.mask & EVENT_HUP || ev.mask & EVENT_ERR) {
                s->op.handle_conn_hup(s, conn);
                continue;
            }
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
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    if (eventloop_add(s->eventloop, fd, EVENT_READ | EVENT_WRITE | EVENT_HUP | EVENT_ERR) < 0) {
        free(conn);
        return -1;
    }
    #ifdef DEBUG
    printf("[DEBUG] conn accepted fd=%d\n", fd);
    #endif
    conn->fd = fd;
    s->conns[fd] = conn;
    return 0;
}

void server_handle_conn_read(struct server *s, struct connection *conn)
{
    char buf[256];
    int n = read(conn->fd, buf, 256);
    #ifdef DEBUG
    printf("[DEBUG] recv from fd=%d dataLen= %ld\n", conn->fd, strlen(buf));
    #endif
    struct resp_cmd req;
    if(decode_resp_cmd(buf, n, &req)==-1) {
        return;
    }
    struct resp_cmd resp;
    if (s->db->handle_command(s->db, conn, &req, &resp)==0) {
        conn->buf_len = encode_resp_cmd(&conn->write_buf, &resp);
    }
    free_resp_cmd(&req);
    free_resp_cmd(&resp);
}

void server_handle_conn_write(struct server *s, struct connection *conn)
{
    if (conn->buf_len == 0 || conn->write_buf == NULL) return;
    int n = write(conn->fd, conn->write_buf, conn->buf_len);
    if (n < 0) {
        printf("write resp error: %s\n", strerror(errno));
        return;
    }
    #ifdef DEBUG
    printf("[DEBUG] send to fd=%d\n", conn->fd);
    #endif
    free(conn->write_buf);
    conn->write_buf = NULL;
    conn->buf_len = 0;
}

void server_handle_conn_hup(struct server *s, struct connection *conn)
{
    #ifdef DEBUG
    printf("[DEBUG]conn closing, fd=%d\n", conn->fd);
    #endif
    eventloop_delete(s->eventloop, conn->fd);
    close(conn->fd);
    s->conns[conn->fd] = NULL;
    free(conn);
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
    s->db = create_database();
    s->fd = sock_fd;
    struct server_op op = {
        .start = server_start,
        .event_handler = server_event_handler,
        .accept = server_accept,
        .shutdown = server_shutdown,
        .handle_conn_read = server_handle_conn_read,
        .handle_conn_write = server_handle_conn_write,
        .handle_conn_hup = server_handle_conn_hup,
    };
    s->op = op;
    return s;
}