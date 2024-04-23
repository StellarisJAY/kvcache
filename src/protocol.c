#include "protocol.h"
#include "str.h"
#include <string.h>
#include <stdlib.h>

#ifdef DEBUG
#include <stdio.h>
#endif

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
    cmd->data = (void *)from_char_array(str, len);
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
    char *num = malloc(num_end - num_start+1);
    num[num_end-num_start] = 0;
    memcpy(num, buf+num_start, num_end-num_start);
    int count = atoi(num);
    free(num);
    if (count <= 0) return -1;

    int nested_start = num_end+2;
    struct resp_cmd_array *arr = malloc(sizeof(struct resp_cmd_array));
    arr->data = malloc(sizeof(void*) * count);
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
        arr->data[i] = temp_cmd.data;
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
        printf("server refuse to read error message\n");
        #endif
        return -1;
    case INT:
        #ifdef DEBUG
        printf("server refuse to read integer message\n");
        #endif
        return -1;
    case BULK_STRING:
        return decode_bulk_string(buf, 1, n, cmd);
    case ARRAY:
        return decode_array(buf, 1, n, cmd);
    default: 
        #ifdef DEBUG
        printf("unknown resp command type:%c\n", buf[0]);
        #endif
        return -1;
    }
}

int encode_resp_cmd(char **buf, struct resp_cmd *cmd)
{
    switch (cmd->type) {
    case SIMPLE_STRING:
        int n = strlen((char *)cmd->data);
        char *res = malloc(n + 5);
        res[0] = cmd->type;
        res[1] = '\r';
        res[2] = '\n';
        memcpy(res+3, cmd->data, n);
        *buf = res;
        return n+5;
    case ERROR:
    case INT:
    case BULK_STRING:
    case ARRAY:
    default:
        return -1;
    }
}