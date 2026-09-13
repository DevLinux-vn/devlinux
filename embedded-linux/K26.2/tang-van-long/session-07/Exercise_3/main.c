#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

#define TRANSACTION_COUNT 5
#define SAFE_DELAY 3
#define IDLE_DELAY 3

int main(void)
{
    sigset_t block_set;
    sigset_t old_set;

    if (sigemptyset(&block_set) == -1)
    {
        perror("sigemptyset");
        return 1;
    }

    if (sigaddset(&block_set, SIGINT) == -1)
    {
        perror("sigaddset");
        return 1;
    }

    for (int transaction = 1;
         transaction <= TRANSACTION_COUNT;
         transaction++)
    {
        if (sigprocmask(SIG_BLOCK, &block_set, &old_set) == -1)
        {
            perror("sigprocmask SIG_BLOCK");
            return 1;
        }

        printf("[SAFE] Writing transaction #%d ...\n", transaction);
        fflush(stdout);

        sleep(SAFE_DELAY);

        printf("[SAFE] Transaction #%d committed.\n", transaction);
        fflush(stdout);

        if (sigprocmask(SIG_SETMASK, &old_set, NULL) == -1)
        {
            perror("sigprocmask SIG_SETMASK");
            return 1;
        }
        printf("[IDLE] Waiting for next transaction...\n");
        fflush(stdout);

        sleep(IDLE_DELAY);
    }

    printf("[INFO] All transactions completed.\n");
    fflush(stdout);

    return 0;
}