#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

volatile sig_atomic_t reading_count = 0;

/* SIGINT handler */
void handle_sigint(int sig)
{
    (void)sig;
    printf("\n[WARN] Received SIGINT, ignoring...\n");
}

/* SIGTERM handler */
void handle_sigterm(int sig)
{
    (void)sig;
    printf("\n[INFO] Received SIGTERM, shutting down gracefully...\n");
    exit(0);
}

/* SIGUSR1 handler */
void handle_sigusr1(int sig)
{
    (void)sig;
    printf("\n[REPORT] Total readings so far: %d\n",
           (int)reading_count);
}

int main(void)
{
    int temperature;

    /* Disable buffering so logs appear immediately */
    setbuf(stdout, NULL);

    /* Register signal handlers */
    signal(SIGINT, handle_sigint);
    signal(SIGTERM, handle_sigterm);
    signal(SIGUSR1, handle_sigusr1);

    printf("[INFO] Sensor daemon started (PID=%d)\n", getpid());

    srand(time(NULL));

    while (1)
    {
        reading_count++;

        temperature = 20 + rand() % 15;

        printf("[INFO] Sensor reading #%d: temperature=%d\n",
               (int)reading_count,
               temperature);

        sleep(1);
    }

    return 0;
}