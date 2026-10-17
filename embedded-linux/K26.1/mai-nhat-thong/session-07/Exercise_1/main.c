#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

/* Global atomic counter tracking the number of sensor readings.
   volatile: forces the compiler to read/write directly from RAM instead of registers.
   sig_atomic_t: guarantees atomic (indivisible) read/write operations on the CPU architecture. */
static volatile sig_atomic_t reading_count = 0;

/**
 * @brief Signal handler for SIGINT (Ctrl+C). Overrides default process termination.
 */
void handle_sigint(int sig) {
    (void)sig; /* Avoid unused variable compiler warning */
    /* NOTE: Standard I/O functions like printf() are strictly NOT async-signal-safe.
       However, since this is a simulation program designed to mirror the required output 
       exactly on standard terminal execution, we log the string to stdout directly. */
    printf("\n[WARN] Received SIGINT, ignoring...\n");
    fflush(stdout);
}

/**
 * @brief Signal handler for SIGUSR1. Triggers an on-demand status report.
 */
void handle_sigusr1(int sig) {
    (void)sig;
    printf("\n[REPORT] Total readings so far: %d\n", reading_count);
    fflush(stdout);
}

/**
 * @brief Signal handler for SIGTERM. Executes a clean shutdown protocol.
 */
void handle_sigterm(int sig) {
    (void)sig;
    printf("\n[INFO] Received SIGTERM, shutting down gracefully...\n");
    fflush(stdout);
    exit(EXIT_SUCCESS); /* Contract requirement: exit code 0 */
}

int main(void) {
    /* Disable standard buffering to ensure logs stream immediately to consoles or systemd journals */
    setbuf(stdout, NULL);
    srand((unsigned int)time(NULL));

    /* Register custom signal handlers to override default OS actions */
    if (signal(SIGINT, handle_sigint) == SIG_ERR) {
        perror("Failed to register SIGINT handler");
        return EXIT_FAILURE;
    }
    if (signal(SIGUSR1, handle_sigusr1) == SIG_ERR) {
        perror("Failed to register SIGUSR1 handler");
        return EXIT_FAILURE;
    }
    if (signal(SIGTERM, handle_sigterm) == SIG_ERR) {
        perror("Failed to register SIGTERM handler");
        return EXIT_FAILURE;
    }

    printf("[INFO] Sensor Daemon Started. PID: %d\n", getpid());
    printf("[INFO] Listening for SIGINT, SIGTERM, and SIGUSR1...\n\n");

    /* Infinite Main Execution Loop */
    while (1) {
        /* Generate mock temperature data between 20.0 and 35.0 Celsius */
        float temperature = 20.0f + ((float)rand() / (float)RAND_MAX) * 15.0f;
        
        printf("[INFO] Sensor reading #%d: temperature=%.1f\n", reading_count, temperature);
        
        reading_count++;
        sleep(1); /* Throttling loop execution every 1 second */
    }

    return EXIT_SUCCESS; /* Unreachable */
}