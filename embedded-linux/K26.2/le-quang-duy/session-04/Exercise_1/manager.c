#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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

void process_order(Order o) {
    float total = o.quantity * o.unit_price;
    printf("[CHILD-%d] PID: %d | PPID: %d\n", o.id, getpid(), getppid());
    printf("[CHILD-%d] %s x%d — Total: %.0f VND\n", o.id, o.name, o.quantity, total);
    printf("[CHILD-%d] Processing... (sleep 2s)\n\n", o.id);
    sleep(2);
}

// Hàm hỗ trợ format tiền tệ có dấu phẩy (VD: 1,560,000)
void print_currency(float amount) {
    int a = (int)amount;
    if (a >= 1000000) {
        printf("%d,%03d,%03d", a / 1000000, (a / 1000) % 1000, a % 1000);
    } else if (a >= 1000) {
        printf("%d,%03d", a / 1000, a % 1000);
    } else {
        printf("%d", a);
    }
}

int main() {
    pid_t pids[3];
    
    printf("===================================================\n");
    printf("   ORDER PROCESSING SYSTEM — MANAGER (fork+wait)\n");
    printf("===================================================\n");
    printf("[MANAGER] PID: %d — spawning 3 child processes...\n\n", getpid());

    // Loop 1: Spawning children
    for (int i = 0; i < 3; i++) {
        fflush(stdout); // Xoá bộ đệm, tránh lỗi in trùng output của cha trong con
        pid_t pid = fork();
        
        if (pid < 0) {
            perror("fork failed");
            exit(1);
        } else if (pid == 0) {
            // Child process
            process_order(orders[i]);
            exit(0);
        } else {
            // Parent process
            pids[i] = pid;
            printf("[MANAGER] fork() order #%d -> child PID: %d\n", i + 1, pid);
        }
    }

    printf("[MANAGER] All 3 children spawned. Starting waitpid()...\n\n");
    printf("--- [child output order may interleave — this is normal] ---\n\n");

    int successful = 0;
    int failed = 0;
    float total_revenue = 0;

    // Loop 2: Waiting for children
    for (int i = 0; i < 3; i++) {
        int status;
        waitpid(pids[i], &status, 0);
        
        if (WIFEXITED(status)) {
            int exit_code = WEXITSTATUS(status);
            printf("[MANAGER] waitpid(%d) — order #%d: exit code=%d -> %s\n",
                   pids[i], i + 1, exit_code, (exit_code == 0) ? "SUCCESS" : "FAILED");
            
            if (exit_code == 0) {
                successful++;
                total_revenue += (orders[i].quantity * orders[i].unit_price);
            } else {
                failed++;
            }
        }
    }

    // Summary
    printf("\n================= SUMMARY =================\n");
    printf("  Total orders    : 3\n");
    printf("  Successful      : %d\n", successful);
    printf("  Failed          : %d\n", failed);
    printf("  Total revenue   : ");
    print_currency(total_revenue);
    printf(" VND\n");
    printf("===========================================\n");

    return 0;
}