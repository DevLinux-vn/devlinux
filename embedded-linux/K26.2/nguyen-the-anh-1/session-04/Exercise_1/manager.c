#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

typedef struct {
    int   id;
    char  name[50];
    int   quantity;
    float unit_price;
} Order;

static void process_order(Order o)
{
    float total = o.quantity * o.unit_price;
    printf("[CHILD-%d] PID: %d | PPID: %d\n", o.id, getpid(), getppid());
    printf("[CHILD-%d] %s x%d — Total: %.0f VND\n",
           o.id, o.name, o.quantity, total);
    printf("[CHILD-%d] Processing... (sleep 2s)\n\n", o.id);
    fflush(stdout);
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
    int   n = 3;

    printf("\n===================================================\n");
    printf("   ORDER PROCESSING SYSTEM — MANAGER (fork+wait)\n");
    printf("===================================================\n");
    printf("[MANAGER] PID: %d — spawning %d child processes...\n\n", getpid(), n);
    fflush(stdout);

    /* ---- Loop 1: fork tat ca con truoc, khong doi ngay ---- */
    for (int i = 0; i < n; i++) {
        fflush(stdout);   /* tranh con thua ke buffer chua flush cua cha */

        pid_t pid = fork();

        if (pid < 0) {
            perror("fork");
            exit(1);
        } else if (pid == 0) {
            /* Process con: xu ly don hang roi thoat */
            process_order(orders[i]);
            exit(0);
        } else {
            /* Process cha: luu lai pid con, KHONG wait ngay */
            pids[i] = pid;
            printf("[MANAGER] fork() order #%d → child PID: %d\n", orders[i].id, pid);
            fflush(stdout);
        }
    }

    printf("[MANAGER] All %d children spawned. Starting waitpid()...\n\n", n);
    printf("--- [child output order may interleave — this is normal] ---\n\n");
    fflush(stdout);

    /* ---- Loop 2: doi tung con theo dung thu tu pids[] ---- */
    int success_count = 0;
    int failed_count  = 0;
    float total_revenue = 0.0f;

    for (int i = 0; i < n; i++) {
        int status;
        waitpid(pids[i], &status, 0);

        float order_total = orders[i].quantity * orders[i].unit_price;

        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            printf("[MANAGER] waitpid(%d) — order #%d: exit code=%d → SUCCESS\n",
                   pids[i], orders[i].id, WEXITSTATUS(status));
            success_count++;
            total_revenue += order_total;
        } else {
            printf("[MANAGER] waitpid(%d) — order #%d: FAILED\n",
                   pids[i], orders[i].id);
            failed_count++;
        }
    }

    printf("\n================= SUMMARY =================\n");
    printf("  Total orders    : %d\n", n);
    printf("  Successful      : %d\n", success_count);
    printf("  Failed          : %d\n", failed_count);
    printf("  Total revenue   : %.0f VND\n", total_revenue);
    printf("===========================================\n");

    return 0;
}
