#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

/*Magic Numbers*/
#define MAX_TRANSACTIONS 5
#define SLEEP_SAFE_SEC 3
#define SLEEP_IDLE_SEC 3
#define SUCCESS_EXIT 0
#define FAIL_EXIT 1

int main(void)
{
    sigset_t block_set, old_set;

    // 1. Init sigset_t
    sigemptyset(&block_set);
    sigaddset(&block_set, SIGINT);

    printf("[INFO] Transaction Logger Started. PID: %d\n", getpid());
    printf("[INFO] Press Ctrl+C during [SAFE] phase to see it pending.\n");
    printf("[INFO] Press Ctrl+C during [IDLE] phase to terminate immediately.\n\n");
    fflush(stdout);

    // 2. Loop MAX_TRANSACTIONS
    for (int i = 1; i <= MAX_TRANSACTIONS; i++)
    {
        // Block SIGINT và save old state
        if (sigprocmask(SIG_BLOCK, &block_set, &old_set) < 0)
        {
            perror("sigprocmask block failed");
            exit(FAIL_EXIT);
        }

        printf("[SAFE] Writing transaction #%d ...\n", i);
        fflush(stdout);

        // simulate write log
        sleep(SLEEP_SAFE_SEC);

        printf("[SAFE] Transaction #%d committed.\n", i);
        fflush(stdout);

        // Restore
        if (sigprocmask(SIG_SETMASK, &old_set, NULL) < 0)
        {
            perror("sigprocmask restore failed");
            exit(FAIL_EXIT);
        }

        if (i < MAX_TRANSACTIONS)
        {
            printf("[IDLE] Waiting for next transaction...\n");
            fflush(stdout);
            sleep(SLEEP_IDLE_SEC);
        }
    }

    printf("\n[INFO] All %d transactions completed successfully.\n", MAX_TRANSACTIONS);
    return SUCCESS_EXIT;
}
