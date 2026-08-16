#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

typedef struct {
    int   id; 
    char  name[50]; 
    int   quantity; 
    float unit_price;
} Order;

/*
 *@brief Process an order in a child process.
 *@param o The order to be processed.
 *@return void
 */
void process_order(Order o) {
    float total = o.quantity * o.unit_price;
    printf("[CHILD-%d] PID: %d | PPID: %d\n", o.id, getpid(), getppid());
    printf("[CHILD-%d] %s x%d — Total: %.0f VND\n",
           o.id, o.name, o.quantity, total);
    printf("[CHILD-%d] Processing... (sleep 2s)\n\n", o.id);
    sleep(2);
}

int main(void) {

    Order orders[3] = {
        {1, "Backpack", 2, 350000},
        {2, "Shoes",    1, 500000},
        {3, "Hat",      3, 120000}
    };

    pid_t pids[3];
    int status;
    int successful = 0;
    int failed = 0;
    float total_revenue = 0.0f;

    printf("===================================================\n");
    printf("   ORDER PROCESSING SYSTEM — MANAGER (fork+wait)\n");
    printf("===================================================\n");
    printf("[MANAGER] PID: %d — spawning 3 child processes...\n\n", getpid());

    // Loop 1: Spawn all child processes concurrently
    for (int i = 0; i < 3; i++) {
        fflush(stdout); // Flush stdout buffer before fork to avoid duplicate logs
        
        pid_t pid = fork();
        
        if (pid < 0) {
            perror("[MANAGER] fork failed");
            pids[i] = -1; /* Mark failure to handle gracefully in Loop 2 */
            failed++;
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Child Process
            process_order(orders[i]);
            exit(0);
        } else {
            // Parent Process
            pids[i] = pid;
            printf("[MANAGER] fork() order #%d → child PID: %d\n", orders[i].id, pid);
        }
    }

    printf("[MANAGER] All 3 children spawned. Starting waitpid()...\n\n");

    // Loop 2: Wait for each child process and collect exit status
    for (int i = 0; i < 3; i++) {

        //reset status variable before each waitpid call
        status = 0;
        
        if (pids[i] < 0) {
            printf("[MANAGER] Skipping waitpid for order #%d (fork failed)\n", orders[i].id);
            continue;
        }

        if (waitpid(pids[i], &status, 0) > 0) {
            if (WIFEXITED(status)) {
                int exit_code = WEXITSTATUS(status);
                printf("[MANAGER] waitpid(%d) — order #%d: exit code=%d → SUCCESS\n",
                       pids[i], orders[i].id, exit_code);
                
                if (exit_code == 0) {
                    successful++;
                    total_revenue += orders[i].quantity * orders[i].unit_price;
                } else {
                    failed++;
                }
            } else {
                printf("[MANAGER] waitpid(%d) — order #%d exited abnormally\n",
                       pids[i], orders[i].id);
                failed++;
            }
        } else {
            perror("[MANAGER] waitpid failed");
            failed++;
        }
    }

    // Print Summary Report
    printf("\n================= SUMMARY =================\n");
    printf("  Total orders    : 3\n");
    printf("  Successful      : %d\n", successful);
    printf("  Failed          : %d\n", failed);
    printf("  Total revenue   : %'.0f VND\n", total_revenue);
    printf("===========================================\n");

    return 0;
}


