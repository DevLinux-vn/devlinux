#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdint.h>

#define SLEEP_INTERVAL_SEC 1

/*
 * volatile sig_atomic_t is safe to modify inside a signal handler.
 * It prevents compiler optimization issues when shared with main loop.
 */
static volatile sig_atomic_t running = 1;

static void handle_sigterm(int sig)
{
    (void)sig;
    running = 0;
}

int main(void)
{
    struct sigaction sa;

    setbuf(stdout, NULL);

    sa.sa_handler = handle_sigterm;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        perror("sigaction");
        return EXIT_FAILURE;
    }

    while (running) {
        printf("Monitor service is running... PID=%d\n", getpid());

        while (sleep(SLEEP_INTERVAL_SEC) > 0) {
            /* Retry sleep if interrupted by signal */
        }
    }

    printf("Service shutting down...\n");
    return EXIT_SUCCESS;
}