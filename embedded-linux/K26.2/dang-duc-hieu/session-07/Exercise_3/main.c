
#define _POSIX_C_SOURCE 200809L

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>

int main()
{
    sigset_t mask, o_mask;

    for (int i = 0; i < 5; i++)
    {
        sigemptyset(&mask);

        sigaddset(&mask, SIGINT);

        if (sigprocmask(SIG_BLOCK, &mask, &o_mask) < 0)
        {
            perror("sigprocmask block failed\n");
            exit(1);
        }

        printf("[SAFE] Writing transaction #%d ...\n", i + 1);
        sleep(3);
        printf("[SAFE] Transaction #%d committed \n", i + 1);

        if (sigprocmask(SIG_SETMASK, &o_mask, NULL) < 0)
        {
            perror("sigprocmask unblock failed\n");
            exit(1);
        }

        printf("[IDLE] Waiting for next transaction...\n");
        sleep(3);
    }

    return 0;
}
