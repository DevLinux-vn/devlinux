#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>

volatile sig_atomic_t sigusr1_received = 0;

void handle_sigusr1(int sig) {
    (void)sig;
    sigusr1_received = 1;
}

int main(void) {
    if (setvbuf(stdout, NULL, _IONBF, 0) != 0) {
        perror("setvbuf");
        return 1;
    }

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_sigusr1;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    sigset_t block_set;
    sigemptyset(&block_set);
    sigaddset(&block_set, SIGUSR1);

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    } else if (pid == 0) {
        // Child
        unsigned int rem = 2;
        while (rem > 0) {
            rem = sleep(rem);
        }
        
        if (kill(getppid(), SIGUSR1) == -1) {
            perror("kill");
            exit(1);
        }
        if (printf("[WORKER] Sent READY signal to gateway\n") < 0) {
            perror("printf");
        }
        exit(7);
    } else {
        // Parent
        if (printf("[GATEWAY] Worker PID = %d\n", pid) < 0) {
            perror("printf");
        }

        // Block SIGUSR1
        if (sigprocmask(SIG_BLOCK, &block_set, NULL) == -1) {
            perror("sigprocmask block");
            return 1;
        }

        // Sleep 5 seconds
        unsigned int rem = 5;
        while (rem > 0) {
            rem = sleep(rem);
        }

        // Unblock SIGUSR1
        if (sigprocmask(SIG_UNBLOCK, &block_set, NULL) == -1) {
            perror("sigprocmask unblock");
            return 1;
        }

        // Check if signal was handled (flag is set by handler)
        if (sigusr1_received) {
            if (printf("[GATEWAY] Worker reported READY signal received\n") < 0) {
                perror("printf");
            }
            sigusr1_received = 0;
        }

        int status;
        if (wait(&status) == -1) {
            perror("wait");
            return 1;
        }

        if (WIFEXITED(status)) {
            if (printf("[GATEWAY] Worker exited with code %d\n", WEXITSTATUS(status)) < 0) {
                perror("printf");
            }
        }
    }

    return 0;
}
