#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
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

    setbuf(stdout, NULL);

    signal(SIGUSR1, handle_sigusr1);

    pid = fork();

    if (pid < 0)
    {
        perror("fork");
        return 1;
    }

    if (pid == 0)
    {
        /* Worker process */
        sleep(2);

        kill(getppid(), SIGUSR1);

        printf("[WORKER] Sent READY signal to gateway\n");

        exit(7);
    }

    /* Gateway process */
    printf("[GATEWAY] Worker PID = %d\n", pid);

    sigset_t block_set;

    sigemptyset(&block_set);
    sigaddset(&block_set, SIGUSR1);

    if (sigprocmask(SIG_BLOCK, &block_set, NULL) == -1)
    {
        perror("sigprocmask SIG_BLOCK");
        return 1;
    }

    sleep(5);

    if (sigprocmask(SIG_UNBLOCK, &block_set, NULL) == -1)
    {
        perror("sigprocmask SIG_UNBLOCK");
        return 1;
    }

    wait(&status);

    if (WIFEXITED(status))
    {
        printf("[GATEWAY] Worker exited with code %d\n",
               WEXITSTATUS(status));
    }

    return 0;
}
