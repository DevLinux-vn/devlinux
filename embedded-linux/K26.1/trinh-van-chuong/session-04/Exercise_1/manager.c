#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

// Định nghĩa cấu trúc Order theo đề bài
typedef struct {
    int   id;
    char  name[50];
    int   quantity;
    float unit_price;
} Order;

// Hàm xử lý đơn hàng của tiến trình con (Design Hints)
void process_order(Order o) {
    float total = o.quantity * o.unit_price;
    printf("[CHILD-%d] PID: %d | PPID: %d\n", o.id, getpid(), getppid());
    printf("[CHILD-%d] %s x%d — Total: %.0f VND\n",
           o.id, o.name, o.quantity, total);
    printf("[CHILD-%d] Processing... (sleep 2s)\n\n", o.id);
    sleep(2);
}

int main() {
    // Khai báo mảng 3 đơn hàng cố định
    Order orders[3] = {
        {1, "Backpack", 2, 350000},
        {2, "Shoes",    1, 500000},
        {3, "Hat",      3, 120000}
    };

    // Mảng để lưu lại PID của 3 tiến trình con sau khi fork
    pid_t pids[3];

    printf("\n===================================================\n");
    printf("    HỆ THỐNG XỬ LÝ ĐƠN HÀNG — MANAGER (fork+wait)\n");
    printf("===================================================\n");
    printf("[MANAGER] PID: %d — spawning 3 child processes...\n\n", getpid());

    // ----------------------------------------------------------------
    // VÒNG LẶP 1: Gọi fork() 3 lần để đẻ ra 3 đứa con chạy song song
    // ----------------------------------------------------------------
    for (int i = 0; i < 3; i++) {
        // fflush(stdout) cực kỳ quan trọng: xóa bộ đệm để tránh việc 
        // tiến trình con vô tình in lặp lại những dòng printf trước đó của cha.
        fflush(stdout); 

        pids[i] = fork();

        if (pids[i] < 0) {
            perror("fork failed");
            exit(1);
        }
        else if (pids[i] == 0) {
            // === KHỐI CODE CỦA TIẾN TRÌNH CON ===
            process_order(orders[i]);
            exit(0); // Con làm xong việc thì CHẾT LUÔN, không đi tiếp vòng lặp!
        }
        else {
            // === KHỐI CODE CỦA TIẾN TRÌNH CHA ===
            // Cha chỉ ghi nhận PID của đứa con vừa đẻ vào mảng rồi tiếp tục lặp
            printf("[MANAGER] fork() order #%d → child PID: %d\n", orders[i].id, pids[i]);
        }
    }

    printf("[MANAGER] All 3 children spawned. Starting waitpid()...\n\n");

    // ----------------------------------------------------------------
    // VÒNG LẶP 2: Chờ và dọn xác (reap) từng đứa con theo đúng thứ tự mảng
    // ----------------------------------------------------------------
    int status;
    int successful_orders = 0;
    int failed_orders = 0;

    for (int i = 0; i < 3; i++) {
        // waitpid chặn cha lại, bắt cha đợi đích danh thằng pids[i] chết
        waitpid(pids[i], &status, 0);

        // Kiểm tra xem con chết bình thường hay bất thường
        if (WIFEXITED(status)) {
            int exit_code = WEXITSTATUS(status);
            printf("[MANAGER] waitpid(%d) — order #%d: exit code=%d → SUCCESS\n", 
                   pids[i], orders[i].id, exit_code);
            successful_orders++;
        } else {
            printf("[MANAGER] waitpid(%d) — order #%d: FAILED\n", pids[i], orders[i].id);
            failed_orders++;
        }
    }

    // ----------------------------------------------------------------
    // TÍNH TOÁN VÀ IN BẢNG TỔNG KẾT (SUMMARY)
    // ----------------------------------------------------------------
    float total_revenue = 0;
    for (int i = 0; i < 3; i++) {
        total_revenue += (orders[i].quantity * orders[i].unit_price);
    }

    printf("\n================= TỔNG KẾT =================\n");
    printf("  Tổng số đơn hàng : 3\n");
    printf("  Thành công       : %d\n", successful_orders);
    printf("  Thất bại         : %d\n", failed_orders);
    // In định dạng số tiền cho đẹp mắt
    printf("  Tổng doanh thu   : %,.0f VND\n", total_revenue);
    printf("===========================================\n");

    return 0;
}