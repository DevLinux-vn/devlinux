#include "moving_average.h"
#include <stdio.h>

int main(void)
{
    ma_filter_t filter;
    ma_init(&filter);
    
    int16_t samples[20] = {
        250, 262, 248, 265, 245, 280, 252, 249,
        251, 247, 253, 290, 250, 248, 252, 245,
        250, 249, 251, 250
    };
    
    printf("========== L12 Lab 4: Moving Average Filter ==========\n\n");
    printf("[!] Feeding 20 noisy temperature ADC samples (Window Size = 4):\n\n");
    printf("Sample # | Raw Temp (0.1C) | Filtered (0.1C) | Status / Notes\n");
    printf("----------------------------------------------------------------------\n");
    
    for (uint32_t i = 0; i < 20U; i++)
    {
        int16_t filtered = ma_process(&filter, samples[i]);
        printf("   %02u    |      %d        |       %d       | ", i + 1, samples[i], filtered);
        
        if (i < 3)
        {
            printf("Startup (%u/4)\n", i + 1);
        }
        else if (i == 3)
        {
            printf("Full Window Reached\n");
        }
        else if (samples[i] >= 280)
        {
            printf("Noise smoothed (Spike)\n");
        }
        else
        {
            printf("Smoothing active\n");
        }
    }
    
    return 0;
}