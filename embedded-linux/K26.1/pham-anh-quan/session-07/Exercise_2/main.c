#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

// Handler xử lý tín hiệu SIGUSR1 của tiến trình Cha (Gateway)
void handle_sigusr1(int sig) {
    (void)sig; // Tránh warning code thừa
    printf("[GATEWAY] Worker reported READY signal received\n");
    fflush(stdout);
}

int main() {
    pid_t pid;
    sigset_t block_set, old_set;

    // 1. Cha thiết lập handler cho SIGUSR1 trước khi fork
    signal(SIGUSR1, handle_sigusr1);

    // Chuẩn bị tập tín hiệu chứa SIGUSR1 để block
    sigemptyset(&block_set);
    sigaddset(&block_set, SIGUSR1);

    // 2. Tạo tiến trình con bằng fork()
    pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        return 1;
    } 
    else if (pid == 0) {
        // ==========================================
        // TIẾN TRÌNH CON (WORKER PROCESS)
        // ==========================================
        // Ngủ 2 giây mô phỏng thời gian khởi tạo worker
        sleep(2);

        // Gửi signal SIGUSR1 lên cho tiến trình cha
        kill(getppid(), SIGUSR1);
        printf("[WORKER] Sent READY signal to gateway\n");
        fflush(stdout);

        // Kết thúc bằng exit(7) theo yêu cầu đề bài
        exit(7);
    } 
    else {
        // ==========================================
        // TIẾN TRÌNH CHA (GATEWAY PROCESS)
        // ==========================================
        printf("[GATEWAY] Worker PID = %d\n", pid);
        fflush(stdout);

        // 3. Block signal SIGUSR1 trong vòng 5 giây
        sigprocmask(SIG_BLOCK, &block_set, &old_set);
        
        // Ngủ 5 giây mô phỏng giai đoạn khởi tạo hệ thống
        sleep(5);

        // 4. Hết 5 giây, thực hiện gỡ block (SIG_UNBLOCK)
        // Nếu SIGUSR1 đang pending, handler sẽ được gọi NGAY LẬP TỨC tại dòng này
        sigprocmask(SIG_UNBLOCK, &block_set, NULL);

        // 5. Chờ con kết thúc bằng wait()
        int status;
        wait(&status);

        // Kiểm tra mã thoát của con bằng WIFEXITED / WEXITSTATUS
        if (WIFEXITED(status)) {
            int exit_code = WEXITSTATUS(status);
            printf("[GATEWAY] Worker exited with code %d\n", exit_code);
        } else {
            printf("[GATEWAY] Worker exited abnormally\n");
        }
        fflush(stdout);
    }

    return 0;
}