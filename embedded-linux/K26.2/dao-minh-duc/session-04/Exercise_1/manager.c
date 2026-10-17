#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
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

printf("[CHILD-%d] PID: %d | PPID: %d\n",
       o.id, getpid(), getppid());

printf("[CHILD-%d] %s x%d - Total: %.0f VND\n",
       o.id, o.name, o.quantity, total);

printf("[CHILD-%d] Processing... (sleep 2s)\n\n",
       o.id);

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
int status;
int successful = 0;
int failed = 0;
float total_revenue = 0.0f;

printf("===================================================\n");
printf("   ORDER PROCESSING SYSTEM - MANAGER (fork+wait)\n");
printf("===================================================\n");

printf("[MANAGER] PID: %d - spawning 3 child processes...\n\n",
       getpid());

/*
 * Loop 1:
 * Create all 3 child processes first.
 *
 * Do NOT put waitpid() here.
 * Otherwise the parent would wait for each child
 * before creating the next one, destroying concurrency.
 */
for (int i = 0; i < 3; i++) {

    /*
     * Make sure stdout buffer is empty before fork().
     * Otherwise buffered parent output may be duplicated
     * by child processes.
     */
    fflush(stdout);

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");

        /*
         * Wait for children that were already created
         * before exiting.
         */
        for (int j = 0; j < i; j++) {
            waitpid(pids[j], NULL, 0);
        }

        return EXIT_FAILURE;
    }

    if (pid == 0) {
        /* Child process */
        process_order(orders[i]);
        exit(0);
    }

    /* Parent process */
    pids[i] = pid;

    printf("[MANAGER] fork() order #%d -> child PID: %d\n",
           orders[i].id, pid);
}

printf("\n[MANAGER] All 3 children spawned. "
       "Starting waitpid()...\n\n");

printf("--- [child output order may interleave - this is normal] ---\n\n");

/*
 * Loop 2:
 * Wait for each child using its PID.
 */
for (int i = 0; i < 3; i++) {

    pid_t result = waitpid(pids[i], &status, 0);

    if (result == -1) {
        perror("waitpid");
        failed++;
        continue;
    }

    if (WIFEXITED(status)) {

        int exit_code = WEXITSTATUS(status);

        if (exit_code == 0) {
            printf("[MANAGER] waitpid(%d) - order #%d: "
                   "exit code=%d -> SUCCESS\n",
                   pids[i],
                   orders[i].id,
                   exit_code);

            successful++;
        } else {
            printf("[MANAGER] waitpid(%d) - order #%d: "
                   "exit code=%d -> FAILED\n",
                   pids[i],
                   orders[i].id,
                   exit_code);

            failed++;
        }

    } else {
        printf("[MANAGER] waitpid(%d) - order #%d: "
               "child did not exit normally\n",
               pids[i],
               orders[i].id);

        failed++;
    }
}

/*
 * Calculate total revenue in the parent.
 */
for (int i = 0; i < 3; i++) {
    total_revenue += orders[i].quantity * orders[i].unit_price;
}

printf("\n================= SUMMARY =================\n");
printf("  Total orders    : 3\n");
printf("  Successful      : %d\n", successful);
printf("  Failed          : %d\n", failed);
printf("  Total revenue   : %.0f VND\n", total_revenue);
printf("===========================================\n");

return EXIT_SUCCESS;

}
