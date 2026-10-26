#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

typedef struct
{
    int id;
    char name[50];
    int quantity;
    float unit_price;
} Order;

void process_order(Order order)
{
    float total;

    total =
        order.quantity *
        order.unit_price;

    printf("[CHILD-%d] PID=%d PPID=%d\n",
           order.id,
           getpid(),
           getppid());

    printf("[CHILD-%d] %s x%d = %.0f VND\n",
           order.id,
           order.name,
           order.quantity,
           total);

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

    int i;

    for (i = 0; i < 3; i++)
    {
        pid_t pid;

        fflush(stdout);

        pid = fork();

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
    }

    for (i = 0; i < 3; i++)
    {
        int status;

        waitpid(pids[i],
                &status,
                0);

        if (WIFEXITED(status))
        {
            printf("[PARENT] Child %d exited with code %d\n",
                   pids[i],
                   WEXITSTATUS(status));
        }
    }

    float revenue = 0;

    for (i = 0; i < 3; i++)
    {
        revenue +=
            orders[i].quantity *
            orders[i].unit_price;
    }

    printf("\n");
    printf("Total Revenue = %.0f VND\n",
           revenue);

    return 0;
}
