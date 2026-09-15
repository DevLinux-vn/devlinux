#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main(void)
{
    sigset_t block_set;
    sigset_t old_set;

    setbuf(stdout, NULL);

    sigemptyset(&block_set);
    sigaddset(&block_set, SIGINT);

    for (int i = 1; i <= 5; i++)
    {
        /* Block SIGINT and save the previous signal mask */
        if (sigprocmask(SIG_BLOCK, &block_set, &old_set) == -1)
        {
            perror("sigprocmask SIG_BLOCK");
            return 1;
        }

        printf("[SAFE] Writing transaction #%d ...\n", i);

        sleep(3);

        printf("[SAFE] Transaction #%d committed.\n", i);

        /* Restore the previous signal mask */
        if (sigprocmask(SIG_SETMASK, &old_set, NULL) == -1)
        {
            perror("sigprocmask SIG_SETMASK");
            return 1;
        }

        printf("[IDLE] Waiting for next transaction...\n");

        sleep(3);
    }

    return 0;
}
