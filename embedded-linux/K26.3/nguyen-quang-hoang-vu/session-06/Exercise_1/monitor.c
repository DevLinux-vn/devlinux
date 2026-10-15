#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

static volatile sig_atomic_t running = 1;

void handle_sigterm(int sig)
{
    (void)sig;

    running = 0;
}

int main(void)
{
    struct sigaction sa;

    sa.sa_handler = handle_sigterm;

    sigemptyset(&sa.sa_mask);

    sa.sa_flags = SA_RESTART;

    if (sigaction(SIGTERM,
                  &sa,
                  NULL) < 0)
    {
        perror("sigaction");

        return 1;
    }

    setbuf(stdout, NULL);

    printf("Monitor service started. PID=%d\n",
           getpid());

    while (running)
    {
        printf("Service is running...\n");

        sleep(1);
    }

    printf("Service shutting down...\n");

    return 0;
}
