#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <string.h>

int main(void) {
    if (setvbuf(stdout, NULL, _IONBF, 0) != 0) {
        perror("setvbuf");
        return 1;
    }

    sigset_t block_set, old_set;
    sigemptyset(&block_set);
    sigaddset(&block_set, SIGINT);

    for (int i = 1; i <= 5; i++) {
        // Block SIGINT and save old mask
        if (sigprocmask(SIG_BLOCK, &block_set, &old_set) == -1) {
            perror("sigprocmask block");
            return 1;
        }

        if (printf("[SAFE] Writing transaction #%d ...\n", i) < 0) {
            perror("printf");
        }

        // Sleep 3 seconds (simulating critical section)
        unsigned int rem = 3;
        while (rem > 0) {
            rem = sleep(rem);
        }

        if (printf("[SAFE] Transaction #%d committed.\n", i) < 0) {
            perror("printf");
        }

        // Restore old mask (if SIGINT was received, it will be delivered now)
        if (sigprocmask(SIG_SETMASK, &old_set, NULL) == -1) {
            perror("sigprocmask restore");
            return 1;
        }
        
        if (printf("[IDLE] Waiting for next transaction...\n") < 0) {
            perror("printf");
        }
        
        // Sleep 3 seconds (idle state)
        rem = 3;
        while (rem > 0) {
            rem = sleep(rem);
        }
    }

    return 0;
}
