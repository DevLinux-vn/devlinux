#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>

typedef struct {
    int   id;
    char  name[50];
    int   quantity;
    float unit_price;
} Order;

Order orders[3] = {
    {1, "Backpack", 2, 350000},
    {2, "Shoes",    1, 500000},
    {3, "Hat",      3, 120000}
};

int pids[3];

void process_order(Order o) {
    float total = o.quantity * o.unit_price;
    printf("[CHILD-%d] PID: %d | PPID: %d\n", o.id, getpid(), getppid());
    printf("[CHILD-%d] %s x%d — Total: %.0f VND\n",
           o.id, o.name, o.quantity, total);
    printf("[CHILD-%d] Processing... (sleep 2s)\n\n", o.id);
    sleep(2);
}

int main() {
    printf("===================================================\n");
    printf("   ORDER PROCESSING SYSTEM — MANAGER (fork+wait)\n");
    printf("===================================================\n");
    printf("[MANAGER] PID: %d — spawning 3 child processes...\n\n", getpid());

    for (int i = 0; i < 3; i++) {
        fflush(stdout); // tránh child in lại buffer chưa flush của parent
        pid_t pid = fork(); // Gọi fork() để tạo tiến trình con
        if (pid == -1) {
            printf("Error when create child process \n");
        } else if (pid == 0) {
            process_order(orders[i]);
            exit(0);
        } else {
            pids[i] = pid;
            printf("[MANAGER] fork() order #%d → child PID: %d\n", orders[i].id, pid);
        }
    }
    printf("[MANAGER] All 3 children spawned. Starting waitpid()...\n\n");

    int status;
    int success = 0;
    float total_revenue = 0;
    for (int i = 0; i < 3; i++) {
        waitpid(pids[i], &status, 0);
        if (WIFEXITED(status)) {
            printf("[MANAGER] waitpid(%d) — order #%d: exit code=%d → SUCCESS\n",
                   pids[i], orders[i].id, WEXITSTATUS(status));
            success++;
            total_revenue += orders[i].quantity * orders[i].unit_price;
        } else if (WIFSIGNALED(status)) {
            printf("[MANAGER] waitpid(%d) — order #%d: killed by signal %d\n",
                   pids[i], orders[i].id, WTERMSIG(status));
        }
    }

    printf("\n================= SUMMARY =================\n");
    printf("  Total orders    : %d\n", 3);
    printf("  Successful      : %d\n", success);
    printf("  Failed          : %d\n", 3 - success);
    printf("  Total revenue   : %.0f VND\n", total_revenue);
    printf("===========================================\n");
    return 0;
}
