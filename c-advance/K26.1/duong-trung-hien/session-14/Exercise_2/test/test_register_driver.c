#include "unity.h"
#include "register_driver.h"

static timer_reg_t g_timer;

void setUp(void) {
    timer_init(&g_timer);
}

void tearDown(void) {}

void test_timer_init_zeros_all(void) {
    TEST_ASSERT_EQUAL_HEX32(0x00000000UL, g_timer.CR);
    TEST_ASSERT_EQUAL_HEX32(0x00000000UL, g_timer.SR);
    TEST_ASSERT_EQUAL_HEX32(0x00000000UL, g_timer.CNT);
    TEST_ASSERT_EQUAL_HEX32(0x00000000UL, g_timer.ARR);
}

void test_timer_configure_prescaler(void) {
    timer_configure(&g_timer, 8U, 1000U);
    uint32_t psc = _FLD2VAL(TIMER_CR_PSC, g_timer.CR);
    TEST_ASSERT_EQUAL_UINT32(8U, psc);
}

void test_timer_configure_reload(void) {
    timer_configure(&g_timer, 4U, 5000U);
    TEST_ASSERT_EQUAL_UINT32(5000U, g_timer.ARR);
}

void test_timer_start_sets_enable(void) {
    timer_start(&g_timer);
    TEST_ASSERT_TRUE((g_timer.CR & TIMER_CR_EN_Msk) != 0U);
}

void test_timer_stop_clears_enable(void) {
    timer_start(&g_timer);
    timer_stop(&g_timer);
    TEST_ASSERT_TRUE((g_timer.CR & TIMER_CR_EN_Msk) == 0U);
}

void test_timer_read_count(void) {
    g_timer.CNT = 500U;  /* Simulate hardware updating the counter */
    TEST_ASSERT_EQUAL_UINT32(500U, timer_read_count(&g_timer));
}

void test_rmw_preserves_other_fields(void) {
    /* Configure prescaler=8 and start timer */
    timer_configure(&g_timer, 8U, 1000U);
    timer_start(&g_timer);
    /* Stop timer — PSC field must be preserved */
    timer_stop(&g_timer);
    uint32_t psc_after = _FLD2VAL(TIMER_CR_PSC, g_timer.CR);
    TEST_ASSERT_EQUAL_UINT32(8U, psc_after);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_timer_init_zeros_all);
    RUN_TEST(test_timer_configure_prescaler);
    RUN_TEST(test_timer_configure_reload);
    RUN_TEST(test_timer_start_sets_enable);
    RUN_TEST(test_timer_stop_clears_enable);
    RUN_TEST(test_timer_read_count);
    RUN_TEST(test_rmw_preserves_other_fields);
    return UNITY_END();
}