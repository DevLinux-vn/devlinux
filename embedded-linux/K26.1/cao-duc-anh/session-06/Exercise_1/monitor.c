#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

static volatile sig_atomic_t running = 1;

static void handle_sigterm(int signum)
{
    (void)signum;
    running = 0;
}

int main(void)
{
    setbuf(stdout, NULL);

    struct sigaction sa;
    sa.sa_handler = handle_sigterm;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGTERM, &sa, NULL);

    unsigned long count = 0;

    while (running) {
        printf("Monitor is running, tick %lu\n", count);
        count++;
        sleep(1);
    }

    printf("Service shutting down...\n");

    return 0;
}
