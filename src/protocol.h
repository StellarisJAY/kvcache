#ifndef PROTOCOL_H
#define PROTOCOL_H

enum resp_type
{
    SIMPLE_STRING='+',
    ERROR='-',
    INT=':',
    BULK_STRING='$',
    ARRAY='*'
};

struct resp_cmd
{
    enum resp_type type;
    void *data;
};

struct resp_cmd_array
{
    unsigned int n;
    // use nested resp_cmd to know data's type
    struct resp_cmd *data;
};

int decode_simple_string(char *buf, int start, int n, struct resp_cmd *cmd);
int decode_bulk_string(char *buf, int start, int n, struct resp_cmd *cmd);
int decode_array(char *buf, int start, int n, struct resp_cmd *cmd);
int decode_resp_cmd(char *buf, int n, struct resp_cmd *cmd);
int encode_resp_cmd(char **buf, struct resp_cmd *cmd);
void free_resp_cmd(struct resp_cmd *cmd);
#endif