#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t reading_count = 0;

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

    printf("[REPORT] Total readings so far: %d\n",
           reading_count);
}

int main(void)
{
    if (signal(SIGINT,
               handle_sigint)
        == SIG_ERR)
    {
        perror("signal");
        return 1;
    }

    if (signal(SIGTERM,
               handle_sigterm)
        == SIG_ERR)
    {
        perror("signal");
        return 1;
    }

    if (signal(SIGUSR1,
               handle_sigusr1)
        == SIG_ERR)
    {
        perror("signal");
        return 1;
    }

    setbuf(stdout,
           NULL);

    printf("[INFO] Sensor daemon started. PID=%d\n",
           getpid());

    while (1)
    {
        reading_count++;

        printf("[INFO] Sensor reading #%d: temp=%d\n",
               reading_count,
               25 + (reading_count % 10));

        sleep(1);
    }

    return 0;
}
