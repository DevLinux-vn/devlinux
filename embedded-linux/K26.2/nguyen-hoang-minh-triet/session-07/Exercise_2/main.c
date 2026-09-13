#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

// Magic number
#define GATEWAY_INIT_SEC 5
#define WORKER_INIT_SEC 2
#define WORKER_EXIT_CODE 7
#define SUCCESS_EXIT 0
#define FAIL_EXIT 1

volatile sig_atomic_t flag_usr1 = 0;

// handler function
void handle_sigusr1(int sig)
{
    // keep warning unused variable
    (void)sig;
    // set flag
    flag_usr1 = 1;
}

int main(void)
{
    // 1. Register signal handlers
    if (signal(SIGUSR1, handle_sigusr1) == SIG_ERR)
    {
        perror("Failed to register SIGUSR1");
        return EXIT_FAILURE;
    }

    // 2. parent create child
    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork failed");
        exit(FAIL_EXIT);
    }
    else if (pid == 0)
    {
        /* ================= Child process (WORKER) ================= */

        // 1. simulate startup
        sleep(WORKER_INIT_SEC);

        // 2. send signal
        kill(getppid(), SIGUSR1);

        // 3. Working
        printf("[WORKER] Sent READY signal to gateway\n");
        fflush(stdout);

        exit(WORKER_EXIT_CODE);
    }
    else
    {
        /* ================= Parent Process (GATEWAY) ================= */

        printf("[GATEWAY] Worker PID = %d\n", pid);
        fflush(stdout);

        sigset_t block_set;
        sigemptyset(&block_set);
        sigaddset(&block_set, SIGUSR1);

        /// 1. block signal
        if (sigprocmask(SIG_BLOCK, &block_set, NULL) < 0)
        {
            perror("sigprocmask block failed");
            exit(FAIL_EXIT);
        }

        // 2. simulate startup
        sleep(GATEWAY_INIT_SEC);

        // 3. unblock
        if (sigprocmask(SIG_UNBLOCK, &block_set, NULL) < 0)
        {
            perror("sigprocmask unblock failed");
            exit(FAIL_EXIT);
        }

        // 4. Working
        if (flag_usr1 == 1)
        {
            printf("[GATEWAY] Worker reported READY signal received\n");
        }

        // 5. wait child process
        int status;
        wait(&status);

        // 6. exit
        if (WIFEXITED(status))
        {
            printf("[GATEWAY] Worker exited with code %d\n", WEXITSTATUS(status));
            fflush(stdout);
        }

        exit(SUCCESS_EXIT);
    }

    return SUCCESS_EXIT;
}
