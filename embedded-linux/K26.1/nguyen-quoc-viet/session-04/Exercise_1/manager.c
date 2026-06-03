#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

typedef struct {
    int   id;
    char  name[50];
    int   quantity;
    float unit_price;
} Order;

void process_order(Order o)
{
    float total = o.quantity * o.unit_price;
    printf("[CHILD-%d] PID: %d | PPID: %d\n", o.id, getpid(), getppid());
    printf("[CHILD-%d] %s x%d — Total: %.0f VND\n",
           o.id, o.name, o.quantity, total);
    printf("[CHILD-%d] Processing... (sleep 2s)\n\n", o.id);
    sleep(2);
}

int main(void)
{
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

    /* Loop 1: fork only — no waitpid here */
    for (int i = 0; i < 3; i++) {
        fflush(stdout);
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(1);
        }
        if (pid == 0) {
            /* child */
            process_order(orders[i]);
            exit(0);
        }
        /* parent */
        pids[i] = pid;
        printf("[MANAGER] fork() order #%d → child PID: %d\n", orders[i].id, pid);
    }

    printf("[MANAGER] All 3 children spawned. Starting waitpid()...\n\n");
    printf("--- [child output order may interleave — this is normal] ---\n\n");

    /* Loop 2: waitpid only — wait for each child by exact PID */
    int successful = 0;
    int failed     = 0;
    float total_revenue = 0;

    for (int i = 0; i < 3; i++) {
        int status;
        waitpid(pids[i], &status, 0);

        if (WIFEXITED(status)) {
            int code = WEXITSTATUS(status);
            printf("[MANAGER] waitpid(%d) — order #%d: exit code=%d → %s\n",
                   pids[i], orders[i].id, code, code == 0 ? "SUCCESS" : "FAILED");
            if (code == 0) {
                successful++;
                total_revenue += orders[i].quantity * orders[i].unit_price;
            } else {
                failed++;
            }
        }
    }

    printf("\n================= SUMMARY =================\n");
    printf("  Total orders    : 3\n");
    printf("  Successful      : %d\n", successful);
    printf("  Failed          : %d\n", failed);
    printf("  Total revenue   : %.0f VND\n", total_revenue);
    printf("===========================================\n");

    return 0;
}
