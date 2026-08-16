#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void handle_sigusr1(int sig)
{
    (void)sig;
    printf("[GATEWAY] Worker reported READY signal received\n");
}

int main(void)
{
    pid_t pid;
    int status;
    sigset_t block_set;

    setbuf(stdout, NULL);

    signal(SIGUSR1, handle_sigusr1);

    pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        sleep(2);

        kill(getppid(), SIGUSR1);
        printf("[WORKER] Sent READY signal to gateway\n");

        exit(7);
    }

    printf("[GATEWAY] Worker PID = %d\n", pid);

    sigemptyset(&block_set);
    sigaddset(&block_set, SIGUSR1);

    sigprocmask(SIG_BLOCK, &block_set, NULL);

    printf("[GATEWAY] Blocking SIGUSR1 for 5 seconds...\n");
    sleep(5);

    printf("[GATEWAY] Unblocking SIGUSR1 now...\n");
    sigprocmask(SIG_UNBLOCK, &block_set, NULL);

    wait(&status);

    if (WIFEXITED(status)) {
        printf("[GATEWAY] Worker exited with code %d\n",
               WEXITSTATUS(status));
    } else {
        printf("[GATEWAY] Worker did not exit normally\n");
    }

    return 0;
}