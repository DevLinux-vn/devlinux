#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

volatile sig_atomic_t keep_running = 1;

void handle_sigterm(int sig) {
    (void)sig;
    keep_running = 0;
}

int main(void) {
    if (setvbuf(stdout, NULL, _IONBF, 0) != 0) {
        fprintf(stderr, "setvbuf failed: %s\n", strerror(errno));
        return 1;
    }

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_sigterm;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        fprintf(stderr, "sigaction failed: %s\n", strerror(errno));
        return 1;
    }

    while (keep_running) {
        printf("Monitor service is running...\n");
        sleep(1);
    }

    printf("Service shutting down...\n");
    return 0;
}
