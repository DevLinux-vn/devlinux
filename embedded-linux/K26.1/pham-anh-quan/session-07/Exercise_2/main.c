#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

// Handler xử lý tín hiệu SIGUSR1 khi cha nhận được báo cáo từ con
void handle_sigusr1(int sig) {
    (void)sig; // Tránh warning unused parameter
    printf("[GATEWAY] worker reported READY signal received\n");
    fflush(stdout);
}

int main() {
    // 1. Cha thiết lập handler cho SIGUSR1 trước khi fork
    signal(SIGUSR1, handle_sigusr1);

    // Khai báo tập hợp tín hiệu để dùng cho việc block
    sigset_t block_set;
    sigemptyset(&block_set);
    sigaddset(&block_set, SIGUSR1);

    // 2. Tạo tiến trình con bằng fork()
    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        return 1;
    } 
    else if (pid == 0) {
        // --- TIẾN TRÌNH CON (WORKER) ---
        // Ngủ 2 giây (mô phỏng thời gian khởi tạo)
        sleep(2);

        // Gửi signal SIGUSR1 lên cho tiến trình cha
        kill(getppid(), SIGUSR1);
        printf("[WORKER] Sent READY signal to gateway\n");
        fflush(stdout);

        // Kết thúc bằng exit(7) theo đúng yêu cầu đề bài
        exit(7);
    } 
    else {
        // --- TIẾN TRÌNH CHA (GATEWAY) ---
        // In ra PID của con vừa tạo
        printf("[GATEWAY] Worker PID = %d\n", pid);
        fflush(stdout);

        // Block signal SIGUSR1 bằng sigprocmask để giả lập khởi tạo hệ thống
        sigprocmask(SIG_BLOCK, &block_set, NULL);
        
        // Ngủ 5 giây (trong lúc này nếu con gửi SIGUSR1 thì sẽ bị giữ ở pending)
        sleep(5);

        // Sau 5 giây, gỡ block (unblock). Lúc này handler sẽ được gọi ngay lập tức
        sigprocmask(SIG_UNBLOCK, &block_set, NULL);

        // Chờ con kết thúc bằng wait()
        int status;
        wait(&status);

        // In ra trạng thái thoát của con sử dụng WIFEXITED và WEXITSTATUS
        if (WIFEXITED(status)) {
            printf("[GATEWAY] Worker exited with code %d\n", WEXITSTATUS(status));
            fflush(stdout);
        }
    }

    return 0;
}