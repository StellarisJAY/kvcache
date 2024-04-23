#ifndef TEST_H
#define TEST_H

void test_hashmap();
void test_database_str();
void test_list();
void test_skiplist();
void test_sorted_set();
void test_threadpool();
void test_decode_resp_cmd();

#ifdef BENCH
void benchmark_hashmap();
#endif
#endif