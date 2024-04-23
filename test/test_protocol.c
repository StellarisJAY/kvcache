#include "protocol.h"
#include "str.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

void test_decode_simple_string()
{
    // test normal
    char *case1 = "+ok\r\n";
    struct resp_cmd cmd;
    int res = decode_resp_cmd(case1, strlen(case1), &cmd);
    assert(res==strlen(case1));
    free(cmd.data);
    // test offset
    char *case2 = "*1\r\n+ok\r\n";
    res = decode_simple_string(case2, 5, strlen(case2), &cmd);
    assert(res == strlen(case2));
    free(cmd.data);
    printf("test decode simple string sucess!\n");
}

void test_decode_bulk_string()
{
    char *case1 = "$10\r\n1234567890\r\n";
    struct resp_cmd cmd;
    int res = decode_resp_cmd(case1, strlen(case1), &cmd);
    assert(res == strlen(case1));
    struct str *data = cmd.data;
    assert(data->length == 10);
    assert(strlen(data->buf) == 10);
    free(data->buf);
    free(cmd.data);

    char *case2 = "*1\r\n$5\r\n12345\r\n";
    res = decode_bulk_string(case2, 5, strlen(case2), &cmd);
    assert(res==strlen(case2));
    data = (struct str*)cmd.data;
    assert(data->length==5);
    assert(strlen(data->buf)==5);
    printf("test decode bulk string success!\n");
}

void test_decode_array()
{
    char *case1 = "*2\r\n$2\r\nok\r\n$2\r\nok\r\n";
    struct resp_cmd cmd;
    int res = decode_resp_cmd(case1, strlen(case1), &cmd);
    assert(res==strlen(case1));
    struct resp_cmd_array *arr = cmd.data;
    assert(arr->n == 2);
    assert(((struct str*)arr->data[0])->length == 2);
    assert(((struct str*)arr->data[1])->length == 2);

    char *case2 = "*2\r\n$7\r\nCOMMAND\r\n$4\r\nDOCS\r\n";
    res = decode_resp_cmd(case2, strlen(case2), &cmd);
    assert(res==strlen(case2));
    printf("test decode array success!\n");
}

void test_decode_resp_cmd()
{
    test_decode_simple_string();
    test_decode_bulk_string();
    test_decode_array();
}