#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main(void) {
    setbuf(stdout, NULL);

    sigset_t block_set, old_set;
    sigemptyset(&block_set);
    sigaddset(&block_set, SIGINT);

    int i;
    for (i = 1; i <= 5; i++) {
        /* ---------------- GIAI ĐOẠN 1: VÙNG AN TOÀN (CRITICAL SECTION) ---------------- */
        /* Block SIGINT and preserve the previous mask state inside old_set */
        if (sigprocmask(SIG_BLOCK, &block_set, &old_set) < 0) {
            perror("Failed to block signals");
            return EXIT_FAILURE;
        }

        printf("[SAFE] Writing transaction #%d ...\n", i);
        sleep(3); /* If Ctrl+C happens here, it becomes pending and deferred */
        printf("[SAFE] Transaction #%d committed.\n", i);

        /* ---------------- GIAI ĐOẠN 2: VÙNG NGHỈ (IDLE ZONE) ---------------- */
        /* Atomically restore the previous mask state instead of a hardcoded unblock */
        if (sigprocmask(SIG_SETMASK, &old_set, NULL) < 0) {
            perror("Failed to restore signal mask");
            return EXIT_FAILURE;
        }

        printf("[IDLE] Waiting for next transaction...\n");
        sleep(3); /* SIGINT is alive here; hitting Ctrl+C terminates the program immediately */
    }

    printf("All transactions processed successfully without interruption.\n");
    return EXIT_SUCCESS;
}