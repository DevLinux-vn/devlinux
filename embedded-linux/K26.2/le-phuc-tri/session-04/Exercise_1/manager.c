#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define NUM_ORDERS 3

typedef struct
{
	int id;
	char name[50];
	int quantity;
	float unit_price;
} Order;

/* Child process handles one order */
void process_order(Order o)
{
	float total = o.quantity * o.unit_price;
	printf("[CHILD-%d] PID: %d | PPID:%d\n",
		       	o.id,getpid(), getppid());
	printf("[CHILD-%d] %s x%d — Total: %.0f VND\n",
           		o.id, o.name, o.quantity, total);

    	printf("[CHILD-%d] Processing... (sleep 2s)\n\n",
           o.id);

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
    int status;

    int successful = 0;
    int failed = 0;

    float total_revenue = 0;


    printf("===================================================\n");
    printf("   ORDER PROCESSING SYSTEM — MANAGER (fork+wait)\n");
    printf("===================================================\n");

    printf("[MANAGER] PID: %d — spawning 3 child processes...\n\n",
           getpid());


    /*
     * LOOP 1:
     * Create all 3 child processes.
     */
    for (int i = 0; i < NUM_ORDERS; i++) {

        /*
         * Make sure the parent's buffered output
         * is not duplicated after fork().
         */
        fflush(stdout);

        pid_t pid = fork();

        if (pid < 0) {
            perror("fork");
            return EXIT_FAILURE;
        }

        if (pid == 0) {
            /*
             * Child process
             */
            process_order(orders[i]);

            /*
             * Child exits with status 0 = success.
             */
            exit(0);
        }

        /*
         * Parent process
         */
        pids[i] = pid;

        printf("[MANAGER] fork() order #%d → child PID: %d\n",
               orders[i].id, pid);
    }


    printf("\n[MANAGER] All 3 children spawned. "
           "Starting waitpid()...\n\n");

    printf("--- [child output order may interleave — this is normal] ---\n\n");


    /*
     * LOOP 2:
     * Wait for each child using the PID saved in pids[].
     */
    for (int i = 0; i < NUM_ORDERS; i++) {

        pid_t result = waitpid(pids[i], &status, 0);

        if (result == -1) {
            perror("waitpid");
            failed++;
            continue;
        }

        /*
         * Check whether child terminated normally.
         */
        if (WIFEXITED(status)) {

            int exit_code = WEXITSTATUS(status);

            if (exit_code == 0) {
                printf("[MANAGER] waitpid(%d) — order #%d: "
                       "exit code=%d → SUCCESS\n",
                       pids[i],
                       orders[i].id,
                       exit_code);

                successful++;

                total_revenue +=
                    orders[i].quantity * orders[i].unit_price;
            }
            else {
                printf("[MANAGER] waitpid(%d) — order #%d: "
                       "exit code=%d → FAILED\n",
                       pids[i],
                       orders[i].id,
                       exit_code);

                failed++;
            }
        }
        else {
            printf("[MANAGER] waitpid(%d) — order #%d: "
                   "did not exit normally → FAILED\n",
                   pids[i],
                   orders[i].id);

            failed++;
        }
    }


    /*
     * Summary
     */
    printf("\n================= SUMMARY =================\n");

    printf("  Total orders    : %d\n", NUM_ORDERS);
    printf("  Successful      : %d\n", successful);
    printf("  Failed          : %d\n", failed);
    printf("  Total revenue   : %.0f VND\n", total_revenue);

    printf("===========================================\n");


    return EXIT_SUCCESS;
}

