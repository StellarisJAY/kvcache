#ifdef TEST
#include "test.h"
int main() 
{
    // test_hashmap();
    // test_database_str();
    // test_list();
    // test_skiplist();
    // test_sorted_set();
    // test_threadpool();
    test_decode_resp_cmd();
    #ifdef BENCH
    benchmark_hashmap();
    #endif
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
    if (s) {
        s->op.start(s);
    }
    
    return 0;
}
#endif