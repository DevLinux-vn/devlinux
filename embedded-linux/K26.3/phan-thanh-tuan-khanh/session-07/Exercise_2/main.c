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
    sigset_t block_set;

    setbuf(stdout, NULL);

    /* Register handler */
    signal(SIGUSR1, handle_sigusr1);

    pid = fork();

    if (pid < 0)
    {
        perror("fork");
        return 1;
    }

    /* Child process */
    if (pid == 0)
    {
        sleep(2);

        kill(getppid(), SIGUSR1);

        printf("[WORKER] Sent READY signal to gateway\n");

        exit(7);
    }

    /* Parent process */
    printf("[GATEWAY] Worker PID = %d\n", pid);

    sigemptyset(&block_set);
    sigaddset(&block_set, SIGUSR1);

    printf("[GATEWAY] Blocking SIGUSR1 for 5 seconds...\n");

    sigprocmask(SIG_BLOCK, &block_set, NULL);

    sleep(5);

    printf("[GATEWAY] Unblocking SIGUSR1...\n");

    sigprocmask(SIG_UNBLOCK, &block_set, NULL);

    if (wait(&status) == -1)
    {
        perror("wait");
        return 1;
    }

    if (WIFEXITED(status))
    {
        printf("[GATEWAY] Worker exited with code %d\n",
               WEXITSTATUS(status));
    }
    else
    {
        printf("[GATEWAY] Worker terminated abnormally\n");
    }

    return 0;
}