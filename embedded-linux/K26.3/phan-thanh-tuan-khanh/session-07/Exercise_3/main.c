#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int main(void)
{
    sigset_t block_set;
    sigset_t old_set;

    setbuf(stdout, NULL);

    /* Prepare signal set containing SIGINT */
    sigemptyset(&block_set);
    sigaddset(&block_set, SIGINT);
    int transaction;
    for (transaction = 1; transaction <= 5; transaction++)
    {
        /* Save current mask and block SIGINT */
        if (sigprocmask(SIG_BLOCK, &block_set, &old_set) < 0)
        {
            perror("sigprocmask(SIG_BLOCK)");
            return 1;
        }

        printf("[SAFE] Writing transaction #%d ...\n", transaction);

        sleep(3);

        printf("[SAFE] Transaction #%d committed.\n", transaction);

        /*
         * Restore previous signal mask instead of
         * manually unblocking SIGINT
         */
        if (sigprocmask(SIG_SETMASK, &old_set, NULL) < 0)
        {
            perror("sigprocmask(SIG_SETMASK)");
            return 1;
        }

        printf("[IDLE] Waiting for next transaction...\n");

        sleep(3);
    }

    printf("[INFO] All transactions completed successfully.\n");

    return 0;
}