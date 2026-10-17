#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

void handle_sigusr1(int sig){
    (void)sig;
    printf("[GATEWAY] Worker reported READY signal received\n");
}

int main() {
    sigset_t block_set;
    int status;
    pid_t child_pid;

    signal(SIGUSR1, handle_sigusr1);

    child_pid = fork();

    if (child_pid < 0) {
        perror("fork failed");
        exit(1);
    } 
    else if (child_pid == 0) {
        sleep(2);
        printf("[WORKER] Sent READY signal to gateway\n");
        kill(getppid(), SIGUSR1);
        exit(7);
    } 
    else {
        printf("[GATEWAY] Worker PID = %d\n", child_pid);

        sigemptyset(&block_set);
        sigaddset(&block_set, SIGUSR1);

        sigprocmask(SIG_BLOCK, &block_set, NULL);
        sleep(5);
        sigprocmask(SIG_UNBLOCK, &block_set, NULL);

        wait(&status);

        if (WIFEXITED(status)) {
            printf("[GATEWAY] Worker exited with code %d\n", WEXITSTATUS(status));
        }

        return 0;
    }
}