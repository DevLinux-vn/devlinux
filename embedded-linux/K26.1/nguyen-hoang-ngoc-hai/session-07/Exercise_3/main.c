#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int main(void)
{
    sigset_t block_set;
    sigset_t old_set;
    int i;

    setbuf(stdout, NULL);

    sigemptyset(&block_set);
    sigaddset(&block_set, SIGINT);

    for (i = 1; i <= 5; i++) {
        if (sigprocmask(SIG_BLOCK, &block_set, &old_set) == -1) {
            perror("sigprocmask block");
            exit(1);
        }

        printf("[SAFE] Writing transaction #%d ...\n", i);
        sleep(3);
        printf("[SAFE] Transaction #%d committed.\n", i);

        if (sigprocmask(SIG_SETMASK, &old_set, NULL) == -1) {
            perror("sigprocmask restore");
            exit(1);
        }

        printf("[IDLE] Waiting for next transaction...\n");
        sleep(3);
    }

    return 0;
}