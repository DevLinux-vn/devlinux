
#include "unity.h"



extern void test_parse_valid_ip(void);
extern void test_parse_invalid_range(void);
extern void test_parse_null_input(void);


int main()
{
    UNITY_BEGIN();
    RUN_TEST(test_parse_valid_ip);
    RUN_TEST(test_parse_invalid_range);
    RUN_TEST(test_parse_null_input);



    return UNITY_END();
}