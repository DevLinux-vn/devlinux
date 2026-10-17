#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include "./signal_handler.h"

volatile sig_atomic_t reading_count = 0;

float get_random_temperature(float min, float max)
{
    // rand() / (float)RAND_MAX generates a value between 0.0 and 1.0
    float scale = rand() / (float)RAND_MAX;

    // Scale and shift the value to the desired range
    return min + scale * (max - min);
}

int main()
{
    signal(SIGINT, handle_sigint);
    signal(SIGTERM, handle_sigterm);
    signal(SIGUSR1, handle_sigusr1);

    while (1)
    {
        float temperature = get_random_temperature(20, 40);
        printf("[INFO] Sensor reading #%d: temperature=%f \n", reading_count, temperature);
        reading_count++;
        sleep(1);
    }
    return 0;
}

void handle_sigint()
{
    signal(SIGINT, handle_sigint);
    printf("[WARN] Received SIGINT, ignoring...\n");
}

void handle_sigterm()
{
    printf("[INFO] Received SIGTERM, shutting down gracefully...\n");
    exit(0);
}

void handle_sigusr1()
{
    signal(SIGUSR1, handle_sigusr1);
    printf("[REPORT] Total readings so far <%d> \n", reading_count);
}