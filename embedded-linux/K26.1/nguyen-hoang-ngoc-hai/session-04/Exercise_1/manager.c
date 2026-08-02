#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define ORDER_COUNT 3

typedef struct {
    int id;
    char name[50];
    int quantity;
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

int main(void) {
    Order orders[ORDER_COUNT] = {
        {1, "Backpack", 2, 350000},
        {2, "Shoes",    1, 500000},
        {3, "Hat",      3, 120000}
    };

    pid_t pids[ORDER_COUNT];
    int status;
    int successful = 0;
    int failed = 0;
    float total_revenue = 0;

    printf("\n===================================================\n");
    printf("   ORDER PROCESSING SYSTEM — MANAGER (fork+wait)\n");
    printf("===================================================\n");
    printf("[MANAGER] PID: %d — spawning %d child processes...\n\n",
           getpid(), ORDER_COUNT);

    /*
     * Loop 1:
     * Fork all child processes first.
     * Do not wait here, otherwise execution becomes sequential.
     */
    for (int i = 0; i < ORDER_COUNT; i++) {
        fflush(stdout);

        pid_t pid = fork();

        if (pid < 0) {
            perror("fork");
            failed++;
            continue;
        }

        if (pid == 0) {
            process_order(orders[i]);
            exit(0);
        }

        pids[i] = pid;
        printf("[MANAGER] fork() order #%d → child PID: %d\n",
               orders[i].id, pid);
    }

    printf("[MANAGER] All %d children spawned. Starting waitpid()...\n\n",
           ORDER_COUNT);
    printf("--- [child output order may interleave — this is normal] ---\n\n");

    /*
     * Loop 2:
     * Wait for each specific child PID.
     */
    for (int i = 0; i < ORDER_COUNT; i++) {
        pid_t waited_pid = waitpid(pids[i], &status, 0);

        if (waited_pid < 0) {
            perror("waitpid");
            failed++;
            continue;
        }

        if (WIFEXITED(status)) {
            int exit_code = WEXITSTATUS(status);

            if (exit_code == 0) {
                successful++;
                total_revenue += orders[i].quantity * orders[i].unit_price;

                printf("[MANAGER] waitpid(%d) — order #%d: exit code=%d → SUCCESS\n",
                       waited_pid, orders[i].id, exit_code);
            } else {
                failed++;

                printf("[MANAGER] waitpid(%d) — order #%d: exit code=%d → FAILED\n",
                       waited_pid, orders[i].id, exit_code);
            }
        } else {
            failed++;

            printf("[MANAGER] waitpid(%d) — order #%d: child did not exit normally\n",
                   waited_pid, orders[i].id);
        }
    }

    printf("\n================= SUMMARY =================\n");
    printf("  Total orders    : %d\n", ORDER_COUNT);
    printf("  Successful      : %d\n", successful);
    printf("  Failed          : %d\n", failed);
    printf("  Total revenue   : %.0f VND\n", total_revenue);
    printf("===========================================\n");

    return 0;
}