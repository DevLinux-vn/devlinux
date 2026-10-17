#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

static volatile sig_atomic_t g_stop = 0;

static void handle_sigterm(int sig)
{
    (void)sig;
    g_stop = 1; /* only set a flag: printf() is not async-signal-safe */
}

int main(void)
{
    struct sigaction sa;
    unsigned long counter = 0;

    setbuf(stdout, NULL); /* log lines appear in the journal immediately */

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_sigterm;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0; /* no SA_RESTART: sleep() is interrupted by the signal */
    if (sigaction(SIGTERM, &sa, NULL) != 0) {
        perror("sigaction");
        return EXIT_FAILURE;
    }

    printf("Service started (PID %d)\n", (int)getpid());

    while (!g_stop) {
        time_t now = time(NULL);
        char buf[32];
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
        printf("[%s] monitor is running, tick %lu\n", buf, ++counter);
        sleep(1);
    }

    printf("Service shutting down...\n");
    return EXIT_SUCCESS;
}
