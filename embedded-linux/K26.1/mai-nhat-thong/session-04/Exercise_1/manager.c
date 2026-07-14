#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/* Order structure definition */
typedef struct {
    int id;
    char name[50];
    int quantity;
    float unit_price;
} Order;

/**
 * @brief Simulates order processing inside the child process.
 * @param o The order to be processed.
 */
void process_order(Order o) {
    float total = o.quantity * o.unit_price;
    
    /* Display child and parent process information */
    printf("[CHILD-%d] PID: %d | PPID: %d\n", o.id, getpid(), getppid());
    printf("[CHILD-%d] %s x%d — Total: %.0f VND\n", o.id, o.name, o.quantity, total);
    printf("[CHILD-%d] Processing... (sleep 2s)\n\n", o.id);
    
    /* Simulate hardware or network latency */
    sleep(2);
}

int main(void) {
    /* Hardcoded array of 3 orders as required */
    Order orders[3] = {
        { 1, "Backpack", 2, 350000 },
        { 2, "Shoes",    1, 500000 },
        { 3, "Hat",      3, 120000 }
    };

    pid_t pids[3];
    int success_count = 0;
    int failed_count = 0;
    float total_revenue = 0;

    printf("===================================================\n");
    printf("   ORDER PROCESSING SYSTEM — MANAGER (fork+wait)\n");
    printf("===================================================\n");
    printf("[MANAGER] PID: %d — spawning 3 child processes...\n\n", getpid());

    /* ---------------------------------------------------------------------- */
    /* LOOP 1: Spawning concurrent child processes                           */
    /* ---------------------------------------------------------------------- */
    for (int i = 0; i < 3; i++) {
        /* CRITICAL: Flush stdout buffer before fork to prevent duplicated logs 
           in child processes if output is redirected to a file or pipe. */
        fflush(stdout);

        pid_t pid = fork();

        if (pid < 0) {
            /* Fork failed */
            perror("[MANAGER] fork failed");
            exit(EXIT_FAILURE);
        } 
        else if (pid == 0) {
            /* Child Process Context */
            process_order(orders[i]);
            /* Terminate child immediately with success status 0 */
            exit(EXIT_SUCCESS);
        } 
        else {
            /* Parent Process Context */
            pids[i] = pid;
            printf("[MANAGER] fork() order #%d → child PID: %d\n", orders[i].id, pid);
        }
    }

    printf("[MANAGER] All 3 children spawned. Starting waitpid()...\n\n");

    /* ---------------------------------------------------------------------- */
    /* LOOP 2: Reaping children and aggregating statistics                   */
    /* ---------------------------------------------------------------------- */
    for (int i = 0; i < 3; i++) {
        int status;
        /* Wait specifically for pids[i] synchronously in the original order */
        pid_t reaped_pid = waitpid(pids[i], &status, 0);

        if (reaped_pid < 0) {
            perror("[MANAGER] waitpid failed");
            failed_count++;
            continue;
        }

        /* Check if the child process terminated normally */
        if (WIFEXITED(status)) {
            int exit_code = WEXITSTATUS(status);
            printf("[MANAGER] waitpid(%d) — order #%d: exit code=%d → ", pids[i], orders[i].id, exit_code);
            
            if (exit_code == 0) {
                printf("SUCCESS\n");
                success_count++;
                total_revenue += (orders[i].quantity * orders[i].unit_price);
            } else {
                printf("FAILED\n");
                failed_count++;
            }
        } else {
            /* Child was terminated abnormally (e.g., killed by a signal) */
            printf("[MANAGER] waitpid(%d) — order #%d terminated abnormally\n", pids[i], orders[i].id);
            failed_count++;
        }
    }

    /* Print Final Summary Report */
    printf("\n================= SUMMARY =================\n");
    printf("  Total orders    : 3\n");
    printf("  Successful      : %d\n", success_count);
    printf("  Failed          : %d\n", failed_count);
    printf("  Total revenue   : %,.0f VND\n", total_revenue);
    printf("===========================================\n");

    return EXIT_SUCCESS;
}