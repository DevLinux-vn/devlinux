#include "unity.h"
#include "ipv4_parser.h"


void setUp(void) {}
void tearDown(void) {}




void test_parse_valid_ip(void)
{
    uint32_t ip_out = 0;
    int8_t ret = parse_ipv4("192.168.1.50", &ip_out);
    TEST_ASSERT_EQUAL_INT8(0, ret);
    TEST_ASSERT_EQUAL_HEX32(0xC0A80132, ip_out);
}


void test_parse_invalid_range(void)
{
    uint32_t ip_out = 0;

    int8_t ret = parse_ipv4("256.0.0.1", &ip_out);
    TEST_ASSERT_EQUAL_INT32(-1, ret);
}

void test_parse_null_input(void)
{
    uint32_t ip_out = 0;

    int8_t ret = parse_ipv4(NULL, &ip_out);
    TEST_ASSERT_EQUAL_INT32(-1, ret);
}
