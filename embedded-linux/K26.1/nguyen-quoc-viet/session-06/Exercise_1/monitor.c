#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

static volatile sig_atomic_t running = 1;

static void handle_sigterm(int sig)
{
    (void)sig;
    running = 0;
}

int main(void)
{
    /* Disable stdout buffering so every log line reaches the journal immediately */
    setbuf(stdout, NULL);

    signal(SIGTERM, handle_sigterm);

    int cycle = 0;
    while (running) {
        time_t now = time(NULL);
        struct tm *t = localtime(&now);
        printf("[%02d:%02d:%02d] monitor service running — cycle %d\n",
               t->tm_hour, t->tm_min, t->tm_sec, ++cycle);
        sleep(1);
    }

    printf("Service shutting down...\n");
    return 0;
}
