#ifdef TEST
#include "test.h"
int main() 
{
    test_hashmap();
    test_database_str();
    return 0;
}
#else
#include <stdlib.h>
#include "server.h"
#define DEFAULT_PORT 6381
int main(int argc, char *argv[])
{   
    int port;
    if (argc < 2) {
        port = DEFAULT_PORT;
    }else {
        port = atoi(argv[1]);
    }
    struct server_config config = {
        .max_conns = 1024,
        .port = port,
    };
    struct server *s = create_server(config);
    s->op.start(s);
    return 0;
}
#endif