#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define ORDER_COUNT 3

typedef struct {
    int id;
    char name[50];
    int quantity;
    float unit_price;
} Order;

static void process_order(Order order)
{
    float total = order.quantity * order.unit_price;

    printf("[CHILD-%d] PID: %d | PPID: %d\n",
           order.id, getpid(), getppid());
    printf("[CHILD-%d] %s x%d - Total: %.0f VND\n",
           order.id, order.name, order.quantity, total);
    printf("[CHILD-%d] Processing... (sleep 2s)\n\n", order.id);
    sleep(2);
}

int main(void)
{
    Order orders[ORDER_COUNT] = {
        {1, "Backpack", 2, 350000},
        {2, "Shoes", 1, 500000},
        {3, "Hat", 3, 120000}
    };
    pid_t pids[ORDER_COUNT];
    int successful = 0;
    int failed = 0;
    float total_revenue = 0.0f;

    printf("===================================================\n");
    printf("   ORDER PROCESSING SYSTEM - MANAGER (fork+wait)\n");
    printf("===================================================\n");
    printf("[MANAGER] PID: %d - spawning %d child processes...\n\n",
           getpid(), ORDER_COUNT);

    for (int i = 0; i < ORDER_COUNT; i++) {
        fflush(stdout);
        pids[i] = fork();

        if (pids[i] < 0) {
            perror("fork");
            failed++;
            continue;
        }

        if (pids[i] == 0) {
            process_order(orders[i]);
            exit(0);
        }

        printf("[MANAGER] fork() order #%d -> child PID: %d\n",
               orders[i].id, pids[i]);
    }

    printf("[MANAGER] All %d children spawned. Starting waitpid()...\n\n",
           ORDER_COUNT);
    printf("--- [child output order may interleave - this is normal] ---\n\n");
    fflush(stdout);

    for (int i = 0; i < ORDER_COUNT; i++) {
        int status;

        if (pids[i] <= 0) {
            continue;
        }

        if (waitpid(pids[i], &status, 0) < 0) {
            perror("waitpid");
            failed++;
            continue;
        }

        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            successful++;
            total_revenue += orders[i].quantity * orders[i].unit_price;
            printf("[MANAGER] waitpid(%d) - order #%d: exit code=0 -> SUCCESS\n",
                   pids[i], orders[i].id);
        } else if (WIFEXITED(status)) {
            failed++;
            printf("[MANAGER] waitpid(%d) - order #%d: exit code=%d -> FAILED\n",
                   pids[i], orders[i].id, WEXITSTATUS(status));
        } else {
            failed++;
            printf("[MANAGER] waitpid(%d) - order #%d: child did not exit normally\n",
                   pids[i], orders[i].id);
        }
    }

    printf("\n================= SUMMARY =================\n");
    printf("  Total orders    : %d\n", ORDER_COUNT);
    printf("  Successful      : %d\n", successful);
    printf("  Failed          : %d\n", failed);
    printf("  Total revenue   : %.0f VND\n", total_revenue);
    printf("===========================================\n");

    return failed == 0 ? 0 : 1;
}
