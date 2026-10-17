#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

void handle_sigusr1(int sig) {
    (void)sig;
    printf("[GATEWAY] Worker reported READY signal received\n");
    fflush(stdout);
}

int main(void) {
    setbuf(stdout, NULL);

    /* Set up handler for SIGUSR1 in the parent context */
    if (signal(SIGUSR1, handle_sigusr1) == SIG_ERR) {
        perror("Signal setup failed");
        return EXIT_FAILURE;
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        /* ================= CHILD PROCESS (WORKER) ================= */
        sleep(2); /* Simulate initialization period */
        
        printf("[WORKER] Sent READY signal to gateway\n");
        fflush(stdout);
        
        if (kill(getppid(), SIGUSR1) < 0) {
            perror("Failed to send signal to parent");
            exit(EXIT_FAILURE);
        }
        
        exit(7); /* Terminate with explicit exit code specified in the assignment */
        
    } else {
        /* ================= PARENT PROCESS (GATEWAY) ================= */
        printf("[GATEWAY] Worker PID = %d\n", pid);

        sigset_t block_set;
        sigemptyset(&block_set);
        sigaddset(&block_set, SIGUSR1);

        /* Block SIGUSR1 to prevent immediate handling during setup */
        if (sigprocmask(SIG_BLOCK, &block_set, NULL) < 0) {
            perror("Failed to block SIGUSR1");
            return EXIT_FAILURE;
        }

        /* Simulate critical boot configuration that shouldn't be interrupted */
        sleep(5);

        /* Unblock SIGUSR1 -> Any pending SIGUSR1 signal triggers its handler immediately here */
        if (sigprocmask(SIG_UNBLOCK, &block_set, NULL) < 0) {
            perror("Failed to unblock SIGUSR1");
            return EXIT_FAILURE;
        }

        /* Wait for worker child thread context to finalize clean exit statistics */
        int status;
        if (wait(&status) < 0) {
            perror("Wait execution failed");
            return EXIT_FAILURE;
        }

        if (WIFEXITED(status)) {
            printf("[GATEWAY] Worker exited with code %d\n", WEXITSTATUS(status));
        } else {
            printf("[GATEWAY] Worker exited abnormally\n");
        }
    }

    return EXIT_SUCCESS;
}