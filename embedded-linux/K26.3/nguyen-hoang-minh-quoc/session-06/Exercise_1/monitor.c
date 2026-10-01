/**
 * @file monitor.c
 * @brief Background monitor service that outputs heartbeat logs and handles SIGTERM.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

static volatile sig_atomic_t g_running = 1;

/**
 * @brief Signal handler for clean termination on SIGTERM/SIGINT.
 */
static void handle_sigterm(int signum)
{
    (void)signum;
    g_running = 0;
}

int main(void)
{
    /* Disable stdout buffering so logs appear immediately in systemd journal */
    setbuf(stdout, NULL);

    /* Setup SIGTERM handler using sigaction */
    struct sigaction sa;
    sa.sa_handler = handle_sigterm;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGTERM, &sa, NULL) < 0) {
        perror("Failed to register SIGTERM handler");
        return EXIT_FAILURE;
    }

    /* Also handle SIGINT for graceful exit during terminal testing */
    if (sigaction(SIGINT, &sa, NULL) < 0) {
        perror("Failed to register SIGINT handler");
        return EXIT_FAILURE;
    }

    printf("Monitor service started (PID: %d)\n", getpid());

    unsigned long cycle = 1;
    while (g_running) {
        time_t now = time(NULL);
        struct tm tm_buf;
        localtime_r(&now, &tm_buf);
        char time_str[32];
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", &tm_buf);

        printf("[%s] [Monitor] System heartbeat - cycle %lu (PID %d)\n",
               time_str, cycle++, getpid());

        sleep(1);
    }

    /* Print required shutdown message and exit cleanly */
    printf("Service shutting down...\n");

    return EXIT_SUCCESS;
}
