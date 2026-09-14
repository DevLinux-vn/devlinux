/**
 * @file main.c
 * @brief Demonstration program for the moving average filter.
 */

#include <stdint.h>
#include <stdio.h>

#include "moving_average.h"

#define SAMPLE_COUNT 20U

int main(void)
{
    ma_filter_t filter;
    static const int16_t samples[SAMPLE_COUNT] =
    {
        250, 262, 248, 265, 245,
        280, 252, 249, 251, 247,
        260, 255, 246, 253, 249,
        251, 257, 244, 250, 252
    };

    uint32_t index;
    int16_t filtered;
    const char *p_note;

    ma_init(&filter);

    (void)printf("========== L12 Lab 4: Moving Average Filter ==========\n\n");
    (void)printf("[!] Feeding 20 noisy temperature ADC samples (Window Size = %u):\n\n",
                 (unsigned int)MA_WINDOW_SIZE);
    (void)printf("Sample # | Raw Temp (0.1C) | Filtered (0.1C) | Status / Notes\n");
    (void)printf("-----------------------------------------------------------------\n");

    for (index = 0U; index < SAMPLE_COUNT; ++index)
    {
        filtered = ma_process(&filter, samples[index]);

        if (index < (MA_WINDOW_SIZE - 1U))
        {
            p_note = "Startup";
        }
        else if (index == (MA_WINDOW_SIZE - 1U))
        {
            p_note = "Full Window Reached";
        }
        else if (samples[index] >= 275)
        {
            p_note = "Noise smoothed";
        }
        else
        {
            p_note = "Smoothing active";
        }

        (void)printf("   %02u    |       %3d         |       %3d         | %s\n",
                     (unsigned int)(index + 1U),
                     (int)samples[index],
                     (int)filtered,
                     p_note);
    }

    return 0;
}
