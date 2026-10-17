#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

static volatile sig_atomic_t keep_running = 1;

void handle_sigterm(int sig) {
    (void)sig;
    keep_running = 0;
}

int main(void) {
    /* Make sure log lines show up in the journal immediately, unbuffered */
    setbuf(stdout, NULL);

    struct sigaction sa;
    sa.sa_handler = handle_sigterm;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGTERM, &sa, NULL);

    long counter = 0;

    while (keep_running) {
        printf("Monitor tick %ld - service is alive (pid=%d)\n", counter, getpid());
        counter++;
        sleep(1);
    }

    printf("Service shutting down...\n");
    return 0;
}
