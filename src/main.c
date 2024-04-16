#ifdef TEST
#include "test.h"
int main() 
{
    test_hashmap();
    test_database_str();
    return 0;
}
#else
int main(int argc, char *argv[])
{   
    return 0;
}
#endif