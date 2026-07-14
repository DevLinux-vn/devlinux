#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main(void) {
    setbuf(stdout, NULL);

    /* Construct signal bitmasks for critical zone protection */
    sigset_t block_set, old_set;
    sigemptyset(&block_set);
    sigaddset(&block_set, SIGINT); /* Guard against Ctrl+C interruptions */

    printf("====================================================\n");
    printf("   TRANSACTION LOGGER (Signal Masking Shield)\n");
    printf("====================================================\n\n");

    /* Execute exactly 5 distinct validation transactions as required */
    for (int i = 1; i <= 5; i++) {
        
        /* ------------------------------------------------------------------ */
        /* STAGE 1: CRITICAL SECTION (SAFE ZONE)                              */
        /* ------------------------------------------------------------------ */
        /* BLOCK SIGINT: Save the current signal mask into 'old_set' dynamically 
           to preserve other existing mask flags instead of overwriting them blindly. */
        if (sigprocmask(SIG_BLOCK, &block_set, &old_set) < 0) {
            perror("sigprocmask block failed");
            return EXIT_FAILURE;
        }

        printf("[SAFE] Writing transaction #%d ...\n", i);
        
        /* Simulate heavy disk write overhead latency. 
           If SIGINT arrives during this sleep, it sits safely in a pending queue. */
        sleep(3);

        printf("[SAFE] Transaction #%d committed.\n", i);

        /* RESTORE MASK: Re-apply the old mask configurations layout.
           If a SIGINT was queued, the kernel interrupts and executes default action 
           (termination) precisely on this boundary line. */
        if (sigprocmask(SIG_SETMASK, &old_set, NULL) < 0) {
            perror("sigprocmask restore failed");
            return EXIT_FAILURE;
        }

        /* ------------------------------------------------------------------ */
        /* STAGE 2: IDLE ZONE (VULNERABLE)                                    */
        /* ------------------------------------------------------------------ */
        printf("[IDLE] Waiting for next transaction...\n");
        
        /* SIGINT is now entirely unprotected. 
           Pressing Ctrl+C here immediately kills the process instantly. */
        sleep(3);
        printf("\n");
    }

    printf("[SUCCESS] All 5 transactions managed smoothly. Exiting normally.\n");
    return EXIT_SUCCESS;
}