#include <stdint.h>
#include <stdio.h>
#include "err_util.h"

#define APP_SUCCESS     (0)
#define ARRAY_SIZE(a)   (uint32_t)(sizeof(a) / sizeof((a)[0]))

int32_t main(void)
{
    // NOLINTNEXTLINE(readability-magic-numbers)
    int32_t err_test[] = { 1, 99 };

    for (uint32_t idx = 0U; idx < ARRAY_SIZE(err_test); idx++)
    {
        (void)printf("Error code %d: %s\n", err_test[idx], decode_error(err_test[idx]));
    }

    return APP_SUCCESS;
}