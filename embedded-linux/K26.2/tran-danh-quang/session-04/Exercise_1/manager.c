#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define NUM_ORDERS 3

typedef struct {
    int   id;
    char  name[50];
    int   quantity;
    float unit_price;
} Order;

Order orders[NUM_ORDERS] = {
    {1, "Backpack", 2, 350000},
    {2, "Shoes",    1, 500000},
    {3, "Hat",      3, 120000}
};

int process_order(Order o) {
    float total = o.quantity * o.unit_price;
    printf("[CHILD-%d] PID: %d | PPID: %d\n", o.id, getpid(), getppid());
    printf("[CHILD-%d] %s x%d — Total: %.0f VND\n",
           o.id, o.name, o.quantity, total);
    printf("[CHILD-%d] Processing... (sleep 2s)\n\n", o.id);
    sleep(2);
    return 0; // Success
}

int main(void) {
    pid_t pids[NUM_ORDERS];
    int successful = 0;
    int failed = 0;
    float total_revenue = 0;

    printf("===================================================\n");
    printf("   ORDER PROCESSING SYSTEM — MANAGER (fork+wait)\n");
    printf("===================================================\n");
    printf("[MANAGER] PID: %d — spawning %d child processes...\n\n", getpid(), NUM_ORDERS);

    // Loop 1: spawn children
    for (int i = 0; i < NUM_ORDERS; i++) {
        fflush(stdout); // prevent child from re-printing buffered output
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork failed");
            // If fork fails, just mark as failed and continue? 
            // Better to exit as per standard error handling for critical paths.
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Child process
            int ret = process_order(orders[i]);
            exit(ret == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
        } else {
            // Parent process
            pids[i] = pid;
            printf("[MANAGER] fork() order #%d → child PID: %d\n", orders[i].id, pid);
        }
    }

    printf("[MANAGER] All %d children spawned. Starting waitpid()...\n\n", NUM_ORDERS);

    // Loop 2: wait for children
    for (int i = 0; i < NUM_ORDERS; i++) {
        int status;
        pid_t wpid = waitpid(pids[i], &status, 0);

        if (wpid == -1) {
            perror("waitpid failed");
            continue;
        }

        if (WIFEXITED(status)) {
            int exit_code = WEXITSTATUS(status);
            if (exit_code == EXIT_SUCCESS) {
                printf("[MANAGER] waitpid(%d) — order #%d: exit code=%d → SUCCESS\n", 
                       wpid, orders[i].id, exit_code);
                successful++;
                total_revenue += (orders[i].quantity * orders[i].unit_price);
            } else {
                printf("[MANAGER] waitpid(%d) — order #%d: exit code=%d → FAILED\n", 
                       wpid, orders[i].id, exit_code);
                failed++;
            }
        }
    }

    // Print summary
    printf("\n================= SUMMARY =================\n");
    printf("  Total orders    : %d\n", NUM_ORDERS);
    printf("  Successful      : %d\n", successful);
    printf("  Failed          : %d\n", failed);
    
    // Formatting revenue with comma separator manually
    long long rev = (long long)total_revenue;
    char buf[50];
    int len = snprintf(buf, sizeof(buf), "%lld", rev);
    
    printf("  Total revenue   : ");
    for (int i = 0; i < len; i++) {
        if (i > 0 && (len - i) % 3 == 0) {
            putchar(',');
        }
        putchar(buf[i]);
    }
    printf(" VND\n");
    printf("===========================================\n");

    return 0;
}
