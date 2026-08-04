#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

volatile sig_atomic_t keep_running = 1;

void handle_sigterm(int sig) {
    (void)sig;
    keep_running = 0;
}

int main(void) {
    setbuf(stdout, NULL);

    struct sigaction sa;
    sa.sa_handler = handle_sigterm;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    while (keep_running) {
        printf("Monitor service is running...\n");
        sleep(1);
    }

    printf("Service shutting down...\n");
    return 0;
}
