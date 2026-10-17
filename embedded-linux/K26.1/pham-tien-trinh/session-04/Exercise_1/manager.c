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

void process_order(Order o)
{
    float total = o.quantity * o.unit_price;

    printf("[CHILD-%d] PID: %d | PPID: %d\n",
           o.id, getpid(), getppid());

    printf("[CHILD-%d] %s x%d - Total: %.0f VND\n",
           o.id, o.name, o.quantity, total);

    printf("[CHILD-%d] Processing... (sleep 2s)\n\n", o.id);

    sleep(2);
}

int main(void)
{
    Order orders[NUM_ORDERS] = {
        {1, "Backpack", 2, 350000},
        {2, "Shoes",    1, 500000},
        {3, "Hat",      3, 120000}
    };

    pid_t pids[NUM_ORDERS];
    int successful = 0;
    int failed = 0;
    float total_revenue = 0;

    printf("===================================================\n");
    printf("   ORDER PROCESSING SYSTEM - MANAGER (fork+wait)\n");
    printf("===================================================\n");
    printf("[MANAGER] PID: %d - spawning %d child processes...\n\n",
           getpid(), NUM_ORDERS);

    for (int i = 0; i < NUM_ORDERS; i++) {
        fflush(stdout);

        pid_t pid = fork();

        if (pid < 0) {
            perror("fork");
            exit(1);
        }

        if (pid == 0) {
            process_order(orders[i]);
            exit(0);
        }

        pids[i] = pid;
        printf("[MANAGER] fork() order #%d -> child PID: %d\n",
               orders[i].id, pid);
    }

    printf("[MANAGER] All %d children spawned. Starting waitpid()...\n\n",
           NUM_ORDERS);

    for (int i = 0; i < NUM_ORDERS; i++) {
        int status;

        if (waitpid(pids[i], &status, 0) == -1) {
            perror("waitpid");
            failed++;
            continue;
        }

        if (WIFEXITED(status)) {
            int code = WEXITSTATUS(status);

            if (code == 0) {
                successful++;
                total_revenue += orders[i].quantity * orders[i].unit_price;
                printf("[MANAGER] waitpid(%d) - order #%d: exit code=%d -> SUCCESS\n",
                       pids[i], orders[i].id, code);
            } else {
                failed++;
                printf("[MANAGER] waitpid(%d) - order #%d: exit code=%d -> FAILED\n",
                       pids[i], orders[i].id, code);
            }
        } else {
            failed++;
            printf("[MANAGER] waitpid(%d) - order #%d: abnormal exit\n",
                   pids[i], orders[i].id);
        }
    }

    printf("\n================= SUMMARY =================\n");
    printf("  Total orders    : %d\n", NUM_ORDERS);
    printf("  Successful      : %d\n", successful);
    printf("  Failed          : %d\n", failed);
    printf("  Total revenue   : %.0f VND\n", total_revenue);
    printf("===========================================\n");

    return 0;
}