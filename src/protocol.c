#include "protocol.h"
#include "str.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int decode_simple_string(char *buf, int start, int n, struct resp_cmd *cmd)
{
    cmd->type = SIMPLE_STRING;
    int end = start;
    for (int i = start; i < n; i++) {
        if (buf[i] == '\r' && buf[i+1] == '\n') {
            end = i;
            break;
        }
    }
    if (start == end) return -1;
    char *str = malloc(end - start);
    memcpy(str, buf+start, end-start);
    cmd->data = (void*)str;
    return end + 2;
}

int decode_bulk_string(char *buf, int start, int n, struct resp_cmd *cmd)
{
    cmd->type = BULK_STRING;
    int num_end = start;
    int num_start = start;
    for (; num_end < n; num_end++) {
        if (buf[num_end] == '\r' && buf[num_end+1] == '\n') {
            break;   
        }
    }
    if (num_end == num_start || num_end == n) return -1;
    char *num = malloc(num_end - num_start + 1);
    num[num_end-num_start] = 0;
    memcpy(num, buf+num_start, num_end-num_start);
    int len = atoi(num);
    free(num);
    if (len <= 0) return -1;
    int str_start = num_end + 2;
    char *str = malloc(len);
    memcpy(str, buf+str_start, len);
    cmd->data = (void *)with_char_array(str, len);
    return str_start + len + 2;
}

int decode_array(char *buf, int start, int n, struct resp_cmd *cmd)
{
    cmd->type = ARRAY;
    int num_start = start;
    int num_end = start;
    for (; num_end < n; num_end++) {
        if (buf[num_end] == '\r' && buf[num_end+1] == '\n') break;
    }
    if (num_end == num_start || num_end == n) return -1;
    // make sure a zero is at the end, so atoi won't return unexpected result
    char *num = malloc(num_end - num_start+1);
    num[num_end-num_start] = 0;
    memcpy(num, buf+num_start, num_end-num_start);
    int count = atoi(num);
    free(num);
    if (count <= 0) return -1;

    int nested_start = num_end+2;
    struct resp_cmd_array *arr = malloc(sizeof(struct resp_cmd_array));
    arr->data = malloc(sizeof(struct resp_cmd) * count);
    arr->n = count;
    struct resp_cmd temp_cmd;
    for (int i = 0; i < count; i++) {
        int res = -1;
        switch (buf[nested_start]) {
        case SIMPLE_STRING:
            res = decode_simple_string(buf, nested_start+1, n, &temp_cmd);
            break;
        case BULK_STRING:
            res = decode_bulk_string(buf, nested_start+1, n, &temp_cmd);
            break;
        default:
            #ifdef DEBUG
            printf("[DEBUG] nested type: %c at %d not allowed\n", buf[nested_start], nested_start);
            #endif
        }
        if (res < 0) goto ERROR;
        arr->data[i] = temp_cmd;
        nested_start = res;
    }
    cmd->data = arr;
    return nested_start;
ERROR:
    free(arr->data);
    free(arr);
    cmd->data = NULL;
    return -1;
}

int decode_resp_cmd(char *buf, int n, struct resp_cmd *cmd)
{
    if (n < 3) return -1;
    switch (buf[0]) {
    case SIMPLE_STRING:
        return decode_simple_string(buf, 1, n, cmd);
    case ERROR:
        #ifdef DEBUG
        printf("[DEBUG] server refuse to read error message\n");
        #endif
        return -1;
    case INT:
        #ifdef DEBUG
        printf("[DEBUG] server refuse to read integer message\n");
        #endif
        return -1;
    case BULK_STRING:
        return decode_bulk_string(buf, 1, n, cmd);
    case ARRAY:
        return decode_array(buf, 1, n, cmd);
    default: 
        #ifdef DEBUG
        printf("[DEBUG] unknown resp command type:%c\n", buf[0]);
        #endif
        return -1;
    }
}

int encode_resp_cmd(char **buf, struct resp_cmd *cmd)
{
    char *res;
    switch (cmd->type) {
    case SIMPLE_STRING:
        int n = strlen((char *)cmd->data);
        res = malloc(n + 3);
        res[0] = cmd->type;
        memcpy(res+1, cmd->data, n);
        *buf = res;
        return n+3;
    case ERROR:
    case INT:
        res = malloc(16);
        res[0] = cmd->type;
        res[15] = 0;
        int num = *(int *)cmd->data;
        sprintf(res+1, "%d\r\n", num);
        *buf = res;
        return strlen(res);
    case BULK_STRING:
    case ARRAY:
    default:
        return -1;
    }
}

void free_resp_cmd(struct resp_cmd *cmd)
{
    switch(cmd->type) {
    case SIMPLE_STRING:
    case ERROR:
    case INT:
        free(cmd->data);
        break;
    case BULK_STRING:
        struct str *s = (struct str*)cmd->data;
        free(s->buf);
        free(s);
        break;
    case ARRAY:
        struct resp_cmd_array *arr = (struct resp_cmd_array*)cmd->data;
        for (int i = 0; i < arr->n; i++) {
            free_resp_cmd(&arr->data[i]);
        }
        free(arr->data);
        free(arr);
        break;
    }
}

void create_int_response(int value, struct resp_cmd *cmd)
{
    int *val = malloc(sizeof(int));
    *val = value;
    cmd->type = INT;
    cmd->data = val;
}
// nocopy create response
void create_simple_response(char *buf, struct resp_cmd *cmd)
{
    cmd->type = SIMPLE_STRING;
    cmd->data = buf;
}

void create_error_response(char *error, struct resp_cmd *cmd)
{
    cmd->type = ERROR;
    cmd->data = error;
}

void create_bulk_response(struct str *bulk, struct resp_cmd *cmd)
{
    cmd->type = BULK_STRING;
    cmd->data = bulk;
}

void create_array_response(struct resp_cmd_array *arr, struct resp_cmd *cmd)
{
    cmd->type = ARRAY;
    cmd->data = arr;
}