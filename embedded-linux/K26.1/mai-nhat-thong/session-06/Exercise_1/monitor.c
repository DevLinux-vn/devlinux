#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

/* Global volatile flag to ensure safe asynchronous signal delivery modification */
static volatile sig_atomic_t keep_running = 1;

/**
 * @brief Clean signal handler for SIGTERM sent by systemd.
 * @param signum Signal number triggered.
 */
void handle_sigterm(int signum) {
    (void)signum; /* Prevent unused variable warning */
    keep_running = 0; /* Graceful exit flag */
}

int main(void) {
    /* CRITICAL: Disable stdout buffering so log lines stream to systemd journal immediately */
    setbuf(stdout, NULL);

    /* Setup POSIX signal action for SIGTERM instead of deprecated signal() */
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_sigterm;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGTERM, &sa, NULL) < 0) {
        perror("Error setting up SIGTERM handler");
        return EXIT_FAILURE;
    }

    printf("[MONITOR] Service initialized. PID: %d\n", getpid());

    int counter = 0;
    while (keep_running) {
        printf("[MONITOR] Service active, running cycle: %d\n", counter++);
        sleep(1); /* Loop interval block */
    }

    /* Required exact termination log pattern */
    printf("Service shutting down...\n");
    return EXIT_SUCCESS;
}