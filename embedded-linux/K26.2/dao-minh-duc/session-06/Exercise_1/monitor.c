#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

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

    printf("Monitor service started. PID: %d\n", getpid());

    while (running) {
        printf("Monitor service is running. PID: %d\n", getpid());
        sleep(1);
    }

    printf("Service shutting down...\n");

    return EXIT_SUCCESS;
}
