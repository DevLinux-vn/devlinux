#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

volatile sig_atomic_t ready_received = 0;

void handle_sigusr1(int sig)
{
    (void)sig;

    ready_received = 1;

    printf(
        "[GATEWAY] Worker reported READY signal received\n");
}

int main(void)
{
    sigset_t set;

    if (signal(SIGUSR1,
               handle_sigusr1)
        == SIG_ERR)
    {
        perror("signal");
        return 1;
    }

    sigemptyset(&set);

    sigaddset(&set,
              SIGUSR1);

    printf(
        "[GATEWAY] Blocking SIGUSR1\n");

    sigprocmask(
        SIG_BLOCK,
        &set,
        NULL);

    pid_t pid;

    pid = fork();

    if (pid < 0)
    {
        perror("fork");
        return 1;
    }

    if (pid == 0)
    {
        sleep(2);

        printf(
            "[WORKER] Sent READY signal to gateway\n");

        kill(getppid(),
             SIGUSR1);

        exit(7);
    }

    printf(
        "[GATEWAY] Waiting 5 seconds...\n");

    sleep(5);

    printf(
        "[GATEWAY] Unblocking SIGUSR1\n");

    sigprocmask(
        SIG_UNBLOCK,
        &set,
        NULL);

    int status;

    waitpid(pid,
            &status,
            0);

    if (WIFEXITED(status))
    {
        printf(
            "[GATEWAY] Worker exit code=%d\n",
            WEXITSTATUS(status));
    }

    if (!ready_received)
    {
        printf(
            "[GATEWAY] READY signal not received\n");
    }

    return 0;
}

