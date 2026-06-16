#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

typedef struct {
    int   id;
    char  name[50];
    int   quantity;
    float unit_price;
} Order;

void process_order(Order o) {
    float total = o.quantity * o.unit_price;
    printf("[CHILD-%d] PID: %d | PPID: %d\n", o.id, getpid(), getppid());
    printf("[CHILD-%d] %s x%d — Total: %.0f VND\n",
           o.id, o.name, o.quantity, total);
    printf("[CHILD-%d] Processing... (sleep 2s)\n\n", o.id);
    sleep(2);
}

int main() {
    Order orders[3] = {
        {1, "Backpack", 2, 350000},
        {2, "Shoes",    1, 500000},
        {3, "Hat",      3, 120000}
    };
    
    pid_t pids[3];
    
    printf("===================================================\n");
    printf("   ORDER PROCESSING SYSTEM — MANAGER (fork+wait)\n");
    printf("===================================================\n");
    printf("[MANAGER] PID: %d — spawning 3 child processes...\n\n", getpid());
    
    // Loop 1: fork() 3 times
    for (int i = 0; i < 3; i++) {
        fflush(stdout);  // Important: flush buffer before fork
        pid_t pid = fork();
        
        if (pid == 0) {
            // Child process
            process_order(orders[i]);
            exit(0);
        } else if (pid > 0) {
            // Parent process
            pids[i] = pid;
            printf("[MANAGER] fork() order #%d → child PID: %d\n", i + 1, pid);
        } else {
            perror("fork failed");
            exit(1);
        }
    }
    
    printf("[MANAGER] All 3 children spawned. Starting waitpid()...\n\n");
    printf("--- [child output order may interleave — this is normal] ---\n\n");
    
    // Loop 2: waitpid() for each child
    int total_orders = 3;
    int successful = 0;
    int failed = 0;
    float total_revenue = 0.0;
    
    for (int i = 0; i < 3; i++) {
        int status;
        waitpid(pids[i], &status, 0);
        
        if (WIFEXITED(status)) {
            int exit_code = WEXITSTATUS(status);
            if (exit_code == 0) {
                successful++;
                printf("[MANAGER] waitpid(%d) — order #%d: exit code=%d → SUCCESS\n", pids[i], i + 1, exit_code);
            } else {
                failed++;
                printf("[MANAGER] waitpid(%d) — order #%d: exit code=%d → FAILED\n", pids[i], i + 1, exit_code);
            }
        }
        
        // Calculate revenue
        total_revenue += orders[i].quantity * orders[i].unit_price;
    }
    
    // Print summary
    printf("\n================= SUMMARY =================\n");
    printf("  Total orders    : %d\n", total_orders);
    printf("  Successful      : %d\n", successful);
    printf("  Failed          : %d\n", failed);
    printf("  Total revenue   : %.0f VND\n", total_revenue);
    printf("==========================================\n");
    
    return 0;
}
