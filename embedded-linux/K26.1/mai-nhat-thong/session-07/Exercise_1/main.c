/**
 * @file main.c
 * @brief Embedded Linux Sensor Daemon Signal Handling Simulation.
 * @note Compliant with POSIX.1-2017 Async-Signal-Safe standards.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <string.h>

/* Global atomic flags to ensure safe asynchronous signal delivery modification.
   volatile: forces the compiler to read/write directly from RAM instead of registers.
   sig_atomic_t: guarantees atomic (indivisible) read/write CPU operations. */
static volatile sig_atomic_t reading_count = 0;
static volatile sig_atomic_t flag_sigint = 0;
static volatile sig_atomic_t flag_sigusr1 = 0;
static volatile sig_atomic_t flag_sigterm = 0;

/**
 * @brief Signal handler for SIGINT (Ctrl+C). 
 * @note Only sets an atomic flag to comply with async-signal-safe constraints.
 */
void handle_sigint(int sig) {
    (void)sig;
    flag_sigint = 1;
}

/**
 * @brief Signal handler for SIGUSR1 (On-demand Status Report).
 */
void handle_sigusr1(int sig) {
    (void)sig;
    flag_sigusr1 = 1;
}

/**
 * @brief Signal handler for SIGTERM (Graceful Shutdown Protocol).
 */
void handle_sigterm(int sig) {
    (void)sig;
    flag_sigterm = 1;
}

int main(void) {
    /* CRITICAL: Disable stdout buffering to prevent duplicate or stuck logs */
    setbuf(stdout, NULL);
    srand((unsigned int)time(NULL));

    /* Initialize sigaction structure to replace the non-portable signal() */
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_flags = SA_RESTART; /* Restart interrupted system calls automatically */
    sigemptyset(&sa.sa_mask);

    /* Register SIGINT */
    sa.sa_handler = handle_sigint;
    if (sigaction(SIGINT, &sa, NULL) < 0) {
        perror("Failed to register SIGINT with sigaction");
        return EXIT_FAILURE;
    }

    /* Register SIGUSR1 */
    sa.sa_handler = handle_sigusr1;
    if (sigaction(SIGUSR1, &sa, NULL) < 0) {
        perror("Failed to register SIGUSR1 with sigaction");
        return EXIT_FAILURE;
    }

    /* Register SIGTERM */
    sa.sa_handler = handle_sigterm;
    if (sigaction(SIGTERM, &sa, NULL) < 0) {
        perror("Failed to register SIGTERM with sigaction");
        return EXIT_FAILURE;
    }

    printf("[INFO] Sensor Daemon Started. PID: %d\n", getpid());
    printf("[INFO] Listening for SIGINT, SIGTERM, and SIGUSR1...\n\n");

    /* Infinite Main Execution Loop */
    while (!flag_sigterm) {
        
        /* ------------------------------------------------------------------ */
        /* DEFERRED SIGNAL PROCESSING ZONE (SAFE FOR PRINTF)                  */
        /* ------------------------------------------------------------------ */
        if (flag_sigint) {
            flag_sigint = 0; /* Reset flag */
            printf("\n[WARN] Received SIGINT, ignoring...\n");
        }

        if (flag_sigusr1) {
            flag_sigusr1 = 0; /* Reset flag */
            printf("\n[REPORT] Total readings so far: %d\n", reading_count);
        }

        /* Generate mock temperature data between 20.0 and 35.0 Celsius */
        float temperature = 20.0f + ((float)rand() / (float)RAND_MAX) * 15.0f;
        printf("[INFO] Sensor reading #%d: temperature=%.1f\n", reading_count, temperature);
        reading_count++;

        /* Handle sleep() interruption safely. 
           If sleep() is interrupted by a signal, it returns the remaining time. 
           We loop until the full 1 second has passed. */
        unsigned int remaining = 1;
        while (remaining > 0 && !flag_sigterm && !flag_sigint && !flag_sigusr1) {
            remaining = sleep(remaining);
        }
    }

    /* ------------------------------------------------------------------ */
    /* CLEANUP GRACEFUL SHUTDOWN ZONE                                     */
    /* ------------------------------------------------------------------ */
    printf("\n[INFO] Received SIGTERM, shutting down gracefully...\n");
    return EXIT_SUCCESS; /* Clean termination with exit code 0 */
}