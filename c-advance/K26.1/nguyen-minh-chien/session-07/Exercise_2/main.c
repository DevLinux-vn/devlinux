#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>

typedef struct {
    int32_t min ;
    int32_t max;
    int32_t average;
}stats_t;

stats_t compute_stats(uint32_t count, ...) {
    stats_t result = {0, 0, 0};
    if (count == 0) return result;
    
    va_list ap;
    va_start(ap, count);
    int32_t sum = 0;
    int32_t tmp = 0;
    int32_t current =0;
    // Loop and use va_arg(ap, int)
    for(uint32_t i = 0; i< count; i++){
        tmp = va_arg(ap,int);
        current = (int32_t)tmp;
        if(i==0)
        {
            result.min = current;
            result.max = current;
        }
        else 
        {
            if(current < result.min) {
                result.min = current;
            }
            if(current > result.max) {
                result.max = current;
            }
        }
        sum = sum + current;
    }
    va_end(ap);
    result.average = (int32_t)(sum/(int32_t)count);
    return result;
}

static void print_stats(const char *label, stats_t s)
{
    printf("%s\n", label);
    printf("Min: %d\n", s.min);
    printf("Max: %d\n", s.max);
    printf("Avg: %d\n", s.average);
}

int main(void)
{
    printf("=== Exercise 2: Variadic Stats ===\n");

    stats_t t1 = compute_stats(5U, 10, -5, 20, 0, 5);
    print_stats("Test 1 (5, 10, -5, 20, 0, 5):", t1);

    stats_t t2 = compute_stats(2U, 42, 42);
    print_stats("Test 2 (2, 42, 42):", t2);

    stats_t t3 = compute_stats(0U);
    print_stats("Test 3 (0 args):", t3);

    return 0;
}