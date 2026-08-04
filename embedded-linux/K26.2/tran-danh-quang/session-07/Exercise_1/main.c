#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <string.h>

volatile sig_atomic_t reading_count = 0;
volatile sig_atomic_t sigint_flag = 0;
volatile sig_atomic_t sigusr1_flag = 0;
volatile sig_atomic_t sigterm_flag = 0;

void handle_sigint(int sig) {
    (void)sig;
    sigint_flag = 1;
}

void handle_sigterm(int sig) {
    (void)sig;
    sigterm_flag = 1;
}

void handle_sigusr1(int sig) {
    (void)sig;
    sigusr1_flag = 1;
}

int main(void) {
    if (setvbuf(stdout, NULL, _IONBF, 0) != 0) {
        fprintf(stderr, "setvbuf failed: %s\n", strerror(errno));
        return 1;
    }

    struct sigaction sa_int, sa_term, sa_usr1;

    memset(&sa_int, 0, sizeof(sa_int));
    sa_int.sa_handler = handle_sigint;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = 0;
    if (sigaction(SIGINT, &sa_int, NULL) == -1) {
        perror("sigaction SIGINT");
        return 1;
    }

    memset(&sa_term, 0, sizeof(sa_term));
    sa_term.sa_handler = handle_sigterm;
    sigemptyset(&sa_term.sa_mask);
    sa_term.sa_flags = 0;
    if (sigaction(SIGTERM, &sa_term, NULL) == -1) {
        perror("sigaction SIGTERM");
        return 1;
    }

    memset(&sa_usr1, 0, sizeof(sa_usr1));
    sa_usr1.sa_handler = handle_sigusr1;
    sigemptyset(&sa_usr1.sa_mask);
    sa_usr1.sa_flags = 0;
    if (sigaction(SIGUSR1, &sa_usr1, NULL) == -1) {
        perror("sigaction SIGUSR1");
        return 1;
    }

    // Seed the PRNG with current time or fallback
    srand(12345);

    while (1) {
        reading_count++;
        if (printf("[INFO] Sensor reading #%d: temperature=%d\n", reading_count, 20 + rand() % 10) < 0) {
            perror("printf");
        }

        unsigned int rem = 1;
        while (rem > 0) {
            rem = sleep(rem);
            
            if (sigterm_flag) {
                if (printf("[INFO] Received SIGTERM, shutting down gracefully...\n") < 0) {
                    perror("printf");
                }
                exit(0);
            }
            if (sigint_flag) {
                if (printf("[WARN] Received SIGINT, ignoring...\n") < 0) {
                    perror("printf");
                }
                sigint_flag = 0;
            }
            if (sigusr1_flag) {
                if (printf("[REPORT] Total readings so far: %d\n", reading_count) < 0) {
                    perror("printf");
                }
                sigusr1_flag = 0;
            }
        }
    }

    return 0;
}
