#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t running = 1;

void handle_sigterm(int sig)
{
    (void)sig;

    printf("Service shutting down...\n");
    running = 0;
}

int main(void)
{
    /* Disable stdout buffering */
    setbuf(stdout, NULL);

    /* Register SIGTERM handler */
    signal(SIGTERM, handle_sigterm);

    printf("Monitor service started\n");

    while (running)
    {
        printf("Monitor heartbeat...\n");
        sleep(1);
    }

    return 0;
}