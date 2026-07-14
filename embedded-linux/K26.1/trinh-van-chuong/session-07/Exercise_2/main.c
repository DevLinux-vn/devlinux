#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

/* ── Signal handler cho SIGUSR1 ở tiến trình cha ── */
void handle_sigusr1(int sig) {
    // Dùng biến tránh cảnh báo unused parameter
    (void)sig; 
    
    // Sử dụng hàm an toàn write() thay vì printf()
    const char msg[] = "[GATEWAY] Worker reported READY signal received\n";
    write(STDOUT_FILENO, msg, strlen(msg));
}

int main(void) {
    // 1. Cha: Thiết lập handler cho SIGUSR1 trước khi fork
    signal(SIGUSR1, handle_sigusr1);

    // 2. Cha: Tạo tiến trình con bằng fork()
    pid_t child_pid = fork();

    if (child_pid < 0) {
        perror("Fork failed");
        return EXIT_FAILURE;
    }

    if (child_pid > 0) {
        /* =========================================================
         * TIẾN TRÌNH CHA (GATEWAY)
         * ========================================================= */
        printf("[GATEWAY] Worker PID = %d\n", child_pid);
        fflush(stdout);

        // 3. Cha: Khởi tạo mask và BLOCK signal SIGUSR1 trong 5 giây
        sigset_t block_set;
        sigemptyset(&block_set);
        sigaddset(&block_set, SIGUSR1);

        // Bắt đầu chặn SIGUSR1
        sigprocmask(SIG_BLOCK, &block_set, NULL);
        
        // Mô phỏng giai đoạn khởi tạo hệ thống không muốn bị làm phiền
        sleep(5); 

        // Gỡ block SIGUSR1 sau 5 giây. 
        // Nếu con đã gửi tín hiệu ở giây thứ 2, handler sẽ bị kích hoạt NGAY TẠI DÒNG NÀY.
        sigprocmask(SIG_UNBLOCK, &block_set, NULL);

        // 4. Cha: Chờ con kết thúc và lấy trạng thái thoát (exit code)
        int status;
        wait(&status);

        if (WIFEXITED(status)) {
            printf("[GATEWAY] Worker exited with code %d\n", WEXITSTATUS(status));
        } else {
            printf("[GATEWAY] Worker terminated abnormally\n");
        }
    } 
    else {
        /* =========================================================
         * TIẾN TRÌNH CON (WORKER)
         * ========================================================= */
        // 5. Con: Ngủ 2 giây mô phỏng thời gian tự khởi tạo phần cứng/dữ liệu
        sleep(2);

        // Gửi signal SIGUSR1 lên cho tiến trình cha (getppid() lấy PID của cha)
        kill(getppid(), SIGUSR1);
        
        printf("[WORKER] Sent READY signal to gateway\n");
        fflush(stdout);

        // Kết thúc với mã exit code 7 đúng yêu cầu của đề bài
        exit(7);
    }

    return EXIT_SUCCESS;
}