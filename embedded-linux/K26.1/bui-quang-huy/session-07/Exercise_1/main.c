#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

/* Use volatile sig_atomic_t for variables accessed inside signal handlers */
volatile sig_atomic_t reading_count = 0;

void handle_sigint(int sig) {
    (void)sig; /* Prevent unused parameter warning */
    /* Note: printf is technically not async-signal-safe, but required by assignment output specifications */
    printf("\n[WARN] Received SIGINT, ignoring...\n");
    fflush(stdout);
}

void handle_sigterm(int sig) {
    (void)sig;
    printf("\n[INFO] Received SIGTERM, shutting down gracefully...\n");
    fflush(stdout);
    exit(EXIT_SUCCESS);
}

void handle_sigusr1(int sig) {
    (void)sig;
    printf("\n[REPORT] Total readings so far: %d\n", reading_count);
    fflush(stdout);
}

int main(void) {
    /* Disable stdout buffering so logs appear immediately in standard streams */
    setbuf(stdout, NULL);
    srand((unsigned int)time(NULL));

    /* Register signal handlers using standard C signal() as requested */
    if (signal(SIGINT, handle_sigint) == SIG_ERR) {
        perror("Failed to register SIGINT");
        return EXIT_FAILURE;
    }
    if (signal(SIGTERM, handle_sigterm) == SIG_ERR) {
        perror("Failed to register SIGTERM");
        return EXIT_FAILURE;
    }
    if (signal(SIGUSR1, handle_sigusr1) == SIG_ERR) {
        perror("Failed to register SIGUSR1");
        return EXIT_FAILURE;
    }

    printf("Sensor daemon started. PID: %d\n", getpid());

    /* Infinite loop simulating sensor querying mechanics */
    while (1) {
        reading_count++;
        /* Generate a mock temperature value between 20 and 34 */
        int temp = 20 + (rand() % 15);
        printf("[INFO] Sensor reading #%d: temperature=%d\n", reading_count, temp);
        sleep(1);
    }

    return EXIT_SUCCESS;
}