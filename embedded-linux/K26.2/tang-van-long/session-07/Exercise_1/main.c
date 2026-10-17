#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <errno.h>

#define TEMP_BASE 20
#define TEMP_RANGE 16

volatile sig_atomic_t reading_count = 0;

void handle_sigint(int sig)
{
    (void)sig;

    printf("[WARN] Received SIGINT, ignoring...\n");
    fflush(stdout);
}

void handle_sigterm(int sig)
{
    (void)sig;

    printf("[INFO] Received SIGTERM, shutting down gracefully...\n");
    fflush(stdout);

    exit(0);
}

void handle_sigusr1(int sig)
{
    (void)sig;

    printf("[REPORT] Total readings so far: %d\n", reading_count);
    fflush(stdout);
}

int main(void)
{
    signal(SIGINT, handle_sigint);
    signal(SIGTERM, handle_sigterm);
    signal(SIGUSR1, handle_sigusr1);

    srand((unsigned int)time(NULL));

    printf("[INFO] Sensor daemon started. PID = %d\n", getpid());
    fflush(stdout);

    while (1)
    {
        int temperature = TEMP_BASE + rand() % TEMP_RANGE;

        reading_count++;

        printf("[INFO] Sensor reading #%d: temperature=%d\n",
               reading_count,
               temperature);
        fflush(stdout);

        unsigned int remaining = sleep(1);

        while (remaining > 0)
        {
            remaining = sleep(remaining);
        }
    }

    return 0;
}