#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

/* Use volatile sig_atomic_t for safe access inside signal handler */
volatile sig_atomic_t keep_running = 1;

void handle_sigterm(int signum) {
    (void)signum; /* Prevent unused parameter warning */
    keep_running = 0;
}

int main(void) {
    /* Disable buffering for stdout to flush logs into systemd journal instantly */
    setbuf(stdout, NULL);

    /* Configure sigaction structure for SIGTERM management */
    struct sigaction action;
    action.sa_handler = handle_sigterm;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    if (sigaction(SIGTERM, &action, NULL) < 0) {
        perror("Failed to register SIGTERM handler");
        return EXIT_FAILURE;
    }

    /* Main background service loop */
    while (keep_running) {
        printf("Service is running and monitoring...\n");
        sleep(1);
    }

    /* Clean shutdown execution block noticed by systemd */
    printf("Service shutting down...\n");

    return EXIT_SUCCESS;
}