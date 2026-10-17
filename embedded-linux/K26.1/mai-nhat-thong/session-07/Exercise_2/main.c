#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

/**
 * @brief Signal handler for SIGUSR1 inside the Gateway (Parent) Process.
 */
void handle_sigusr1(int sig) {
    (void)sig;
    printf("[GATEWAY] Worker reported READY signal received\n");
    fflush(stdout);
}

int main(void) {
    setbuf(stdout, NULL);

    /* Step 1: Bind the custom handler for SIGUSR1 in the parent context */
    if (signal(SIGUSR1, handle_sigusr1) == SIG_ERR) {
        perror("[GATEWAY] Failed to set SIGUSR1 handler");
        return EXIT_FAILURE;
    }

    /* Step 2: Spawn the concurrent Worker process */
    pid_t pid = fork();

    if (pid < 0) {
        perror("[GATEWAY] Fork failed");
        return EXIT_FAILURE;
    } 
    else if (pid == 0) {
        /* ------------------------------------------------------------------ */
        /* WORKER PROCESS (CHILD CONTEXT)                                     */
        /* ------------------------------------------------------------------ */
        /* Simulate worker internal setup delay */
        sleep(2);

        /* Send notification signal back to the parent process */
        printf("[WORKER] Sent READY signal to gateway\n");
        fflush(stdout);
        kill(getppid(), SIGUSR1);

        /* Terminate explicitly with status code 7 as mandated by spec */
        exit(7);
    } 
    else {
        /* ------------------------------------------------------------------ */
        /* GATEWAY PROCESS (PARENT CONTEXT)                                   */
        /* ------------------------------------------------------------------ */
        printf("[GATEWAY] Worker PID = %d\n", pid);

        /* Initialize a signal set containing SIGUSR1 to lock out early signals */
        sigset_t block_set;
        sigemptyset(&block_set);
        sigaddset(&block_set, SIGUSR1);

        /* Apply the mask to block SIGUSR1 during the initialization block.
           This pushes any arriving SIGUSR1 signals into a PENDING kernel state. */
        printf("[GATEWAY] Blocking SIGUSR1 for 5 seconds (Initialization phase)...\n");
        if (sigprocmask(SIG_BLOCK, &block_set, NULL) < 0) {
            perror("[GATEWAY] sigprocmask block failed");
            return EXIT_FAILURE;
        }

        /* Simulate a heavy initialization timeframe (5 seconds) */
        sleep(5);

        /* Unblock the signal. The operating system kernel immediately delivers 
           the pending SIGUSR1 signal to the handler here before this function progresses. */
        printf("[GATEWAY] Unblocking SIGUSR1 now.\n");
        if (sigprocmask(SIG_UNBLOCK, &block_set, NULL) < 0) {
            perror("[GATEWAY] sigprocmask unblock failed");
            return EXIT_FAILURE;
        }

        /* Synchronously wait for the worker process to fully terminate */
        int status;
        pid_t reaped_pid = wait(&status);

        if (reaped_pid > 0) {
            if (WIFEXITED(status)) {
                printf("[GATEWAY] Worker exited with code %d\n", WEXITSTATUS(status));
            } else {
                printf("[GATEWAY] Worker terminated abnormally\n");
            }
        }
    }

    return EXIT_SUCCESS;
}