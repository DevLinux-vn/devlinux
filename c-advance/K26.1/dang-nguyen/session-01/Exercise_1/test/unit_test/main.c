#include <stdint.h>
#include <stdio.h>
#include "ut_common.h"
#include "validate_ip_boundary.h"
#include "convert_octet.h"
#include "move_to_next_octet.h"

int32_t main(void)
{
    uint32_t fail_count = 0U;
    uint32_t total_tests = 0U;
    uint32_t test_idx = 1U;
    int32_t exit_code = TEST_EXIT_SUCCESS;

    fail_count += run_validate_ip_boundary_tests(&test_idx);
    fail_count += run_convert_octet_tests(&test_idx);
    fail_count += run_move_to_next_octet_tests(&test_idx);

    total_tests = get_validate_ip_boundary_test_count() +
                  get_convert_octet_test_count() +
                  get_move_to_next_octet_test_count();

    printf("\nTotal unit tests : %u\n", total_tests);
    printf("Failed           : %u\n", fail_count);

    if (0U != fail_count)
    {
        exit_code = TEST_EXIT_FAILURE;
    }

    return exit_code;
}