#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

static volatile sig_atomic_t reading_count = 0;

void handle_sigint(int sig)
{
    (void)sig;
    printf("[WARN] Received SIGINT, ignoring...\n");
}

void handle_sigterm(int sig)
{
    (void)sig;
    printf("[INFO] Received SIGTERM, shutting down gracefully...\n");
    exit(0);
}

void handle_sigusr1(int sig)
{
    (void)sig;
    printf("[REPORT] Total readings so far: %d\n", reading_count);
}

int main(void)
{
    int temperature;

    setbuf(stdout, NULL);
    srand((unsigned int)time(NULL));

    signal(SIGINT, handle_sigint);
    signal(SIGTERM, handle_sigterm);
    signal(SIGUSR1, handle_sigusr1);

    while (1) {
        reading_count++;
        temperature = 25 + rand() % 10;

        printf("[INFO] Sensor reading #%d: temperature=%d\n",
               reading_count,
               temperature);

        sleep(1);
    }

    return 0;
}