#include <stdint.h>
#include <stdio.h>
#include "moving_average.h"

#define APP_SUCCESS         (0U)
#define BUFFER_SIZE         (8U)
#define STRING_SIZE         (3U)

int32_t main() {
    printf("========== L12 Lab 4: Moving Average Filter ==========\n");
    printf("\n[!] Feeding 20 noisy temperature ADC samples (Window Size = 4):\n");
    printf("\nSample # | Raw Temp (0.1°C) | Filtered (0.1°C) | Status / Notes\n");
    printf("-----------------------------------------------------------------\n");

    ma_filter_t filt;
    ma_init(&filt);

    const int16_t buffer[BUFFER_SIZE] = {250U, 262U, 248U, 265U, 245U, 280U, 252U, 249U};
    const char *str[STRING_SIZE] = {"Noise smoothed", "Smoothing active", "Spike evicted"};
    int16_t filtered_result = 0;
    size_t i = 0U;

    for (; i < MA_WINDOW_SIZE; i++) {
        filtered_result = ma_process(&filt, buffer[i]);

        if (i < MA_WINDOW_SIZE - 1U) {
            printf("0%zu    |       %d        |       %d        | Startup (%zu/4)\n", i + 1U, buffer[i], filtered_result, i + 1U);
        }
        else {
            printf("0%zu    |       %d        |       %d        | Full Window Reached\n", i + 1U, buffer[i], filtered_result);
        }
    }

    int16_t old_data = filt.buffer[filt.head];
    
    filtered_result = ma_process(&filt, buffer[i]);
    printf("0%zu    |       %d        |       %d        | Oldest %d replaced\n", i + 1U, buffer[i], filtered_result, old_data);
    i++;

    for (; i < BUFFER_SIZE; i++) {
        filtered_result = ma_process(&filt, buffer[i]);
        printf("0%zu    |       %d        |       %d        | %s\n", i + 1U, buffer[i], filtered_result, str[i - 5U]);
    }

    return APP_SUCCESS;
}