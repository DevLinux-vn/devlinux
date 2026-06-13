#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define ORDER_COUNT     3U

typedef struct
{
    int id;
    char name[50];
    int quantity;
    float unit_price;
} Order;


static void process_order(const Order *order)
{
    float total_price = (float)order->quantity * order->unit_price;

    printf("[CHILD-%d] PID: %d | PPID: %d\n",
           order->id,
           getpid(),
           getppid());

    printf("[CHILD-%d] %s x%d - Total: %.0f VND\n",
           order->id,
           order->name,
           order->quantity,
           total_price);

    printf("[CHILD-%d] Processing... (sleep 2s)\n\n",
           order->id);

    sleep(2);
}


static float calculate_revenue(const Order orders[],
                               size_t size)
{
    float revenue = 0.0F;

    for (size_t i = 0U; i < size; i++)
    {
        revenue += orders[i].quantity * orders[i].unit_price;
    }

    return revenue;
}


int main(void)
{
    Order orders[ORDER_COUNT] =
    {
        {1, "Backpack", 2, 350000},
        {2, "Shoes",    1, 500000},
        {3, "Hat",      3, 120000}
    };


    pid_t pids[ORDER_COUNT];

    int success_count = 0;
    int failed_count = 0;


    printf("=====================================\n");
    printf(" ORDER PROCESSING SYSTEM - MANAGER\n");
    printf("=====================================\n");

    printf("[MANAGER] PID: %d\n\n", getpid());


    /*
     * Loop 1:
     * Create all children first to allow concurrent execution.
     */
    for (size_t i = 0U; i < ORDER_COUNT; i++)
    {
        fflush(stdout);

        pid_t pid = fork();

        if (pid < 0)
        {
            perror("fork failed");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {
            process_order(&orders[i]);
            exit(EXIT_SUCCESS);
        }
        else
        {
            pids[i] = pid;

            printf("[MANAGER] Created child PID: %d for order #%d\n",
                    pid,
                    orders[i].id);
        }
    }


    printf("\n[MANAGER] Waiting children...\n\n");


    /*
     * Loop 2:
     * Wait each specific child using stored PID.
     */
    for (size_t i = 0U; i < ORDER_COUNT; i++)
    {
        int status = 0;

        pid_t result = waitpid(pids[i], &status, 0);

        if (result == -1)
        {
            perror("waitpid failed");
            failed_count++;
            continue;
        }


        if (WIFEXITED(status))
        {
            int exit_code = WEXITSTATUS(status);

            if (exit_code == EXIT_SUCCESS)
            {
                success_count++;
            }
            else
            {
                failed_count++;
            }


            printf("[MANAGER] Child PID %d exited with code %d\n",
                    result,
                    exit_code);
        }
        else
        {
            failed_count++;

            printf("[MANAGER] Child PID %d terminated abnormally\n",
                    result);
        }
    }


    printf("\n=========== SUMMARY ===========\n");

    printf("Total orders : %u\n",
            ORDER_COUNT);

    printf("Success      : %d\n",
            success_count);

    printf("Failed       : %d\n",
            failed_count);

    printf("Revenue      : %.0f VND\n",
            calculate_revenue(orders, ORDER_COUNT));

    printf("===============================\n");


    return EXIT_SUCCESS;
}