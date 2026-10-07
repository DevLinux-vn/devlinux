#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

typedef struct
{
    int id;
    char name[50];
    int quantity;
    float unit_price;
} Order;

void process_order(Order o)
{
    float total = o.quantity * o.unit_price;

    printf("[CHILD-%d] PID: %d | PPID: %d\n",
           o.id,
           getpid(),
           getppid());

    printf("[CHILD-%d] %s x%d - Total: %.0f VND\n",
           o.id,
           o.name,
           o.quantity,
           total);

    printf("[CHILD-%d] Processing... (sleep 2s)\n",
           o.id);

    sleep(2);
}

int main(void)
{
    Order orders[3] =
    {
        {1, "Backpack", 2, 350000},
        {2, "Shoes",    1, 500000},
        {3, "Hat",      3, 120000}
    };

    pid_t pids[3];
    int status;
    float total_revenue = 0.0f;

    printf("========================================\n");
    printf("ORDER PROCESSING SYSTEM - MANAGER\n");
    printf("========================================\n\n");

    printf("[MANAGER] PID: %d - spawning 3 child processes...\n\n",
           getpid());

    /* Loop 1: fork */
    for (int i = 0; i < 3; i++)
    {
        fflush(stdout);

        pid_t pid = fork();

        if (pid < 0)
        {
            perror("fork");
            return 1;
        }

        if (pid == 0)
        {
            process_order(orders[i]);
            exit(0);
        }

        pids[i] = pid;

        printf("[MANAGER] fork() order #%d -> child PID: %d\n",
               i + 1,
               pid);

        total_revenue +=
            orders[i].quantity * orders[i].unit_price;
    }

    printf("[MANAGER] All 3 children spawned. Starting waitpid()...\n\n");

    /* Loop 2: waitpid */
    for (int i = 0; i < 3; i++)
    {
        if (waitpid(pids[i], &status, 0) == -1)
        {
            perror("waitpid");
            continue;
        }

        if (WIFEXITED(status))
        {
            printf("[MANAGER] waitpid(%d) -> order #%d: "
                   "exit code=%d SUCCESS\n",
                   pids[i],
                   i + 1,
                   WEXITSTATUS(status));
        }
        else
        {
            printf("[MANAGER] waitpid(%d) -> order #%d FAILED\n",
                   pids[i],
                   i + 1);
        }
    }

    printf("\n========== SUMMARY ==========\n");
    printf("Total orders : %d\n", 3);
    printf("Successful   : %d\n", 3);
    printf("Failed       : %d\n", 0);
    printf("Total revenue: %.0f VND\n",
           total_revenue);

    return 0;
}