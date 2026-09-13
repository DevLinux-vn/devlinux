#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main(void)
{
    sigset_t block_set;
    sigset_t old_set;

    sigemptyset(&block_set);
    sigaddset(&block_set, SIGINT);

    for (int i = 1; i <= 5; i++)
    {
        if (sigprocmask(SIG_BLOCK, &block_set, &old_set) == -1)
        {
            perror("sigprocmask SIG_BLOCK");
            return 1;
        }

        printf("[SAFE] Writing transaction #%d ...\n", i);
        fflush(stdout);

        sleep(3);

        printf("[SAFE] Transaction #%d committed.\n", i);
        fflush(stdout);

        if (sigprocmask(SIG_SETMASK, &old_set, NULL) == -1)
        {
            perror("sigprocmask SIG_SETMASK");
            return 1;
        }

        printf("[IDLE] Waiting for next transaction...\n");
        fflush(stdout);

        sleep(3);
    }

    return 0;
}