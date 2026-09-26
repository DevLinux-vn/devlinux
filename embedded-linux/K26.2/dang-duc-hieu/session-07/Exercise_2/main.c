
#define _POSIX_C_SOURCE 200809L

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>

void handle_sigusr1();

void handleMainProcess()
{
    sigset_t mask;

    sigemptyset(&mask);

    sigaddset(&mask, SIGUSR1);

    if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0)
    {
        perror("[GATEWAY] sigprocmask block failed\n");
        exit(1);
    }
    sleep(5);
    if (sigprocmask(SIG_UNBLOCK, &mask, NULL) < 0)
    {
        perror("[GATEWAY] sigprocmask unblock failed\n");
        exit(1);
    }
    int status;

    wait(&status);

    if (WIFEXITED(status) && WEXITSTATUS(status) != 1)
    {
        printf("[GATEWAY] Worker exited with code %d\n", WEXITSTATUS(status));
    }
    else
    {
        printf("[GATEWAY] Worker exited abnormally.\n");
    }
}

void handleChildProcess()
{
    sleep(2);
    kill(getppid(), SIGUSR1);
    printf("[WORKER] Sent READY signal to gateway \n");
    exit(7);
}

int main()
{
    signal(SIGUSR1, handle_sigusr1);
    int pid = fork();
    if (pid < 0)
    {
        fprintf(stderr, "[GATEWAY] Fork failed!\n");
        exit(1);
    }
    else if (pid == 0)
    {
        printf("[GATEWAY] Worker PID: %d\n", getpid());
        handleChildProcess();
    }
    else
    {
        handleMainProcess();
    }
}

void handle_sigusr1()
{
    printf("[GATEWAY] Worker reported READY signal received \n");
}