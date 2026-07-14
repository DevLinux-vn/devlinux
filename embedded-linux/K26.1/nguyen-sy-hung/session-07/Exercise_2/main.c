#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define GATEWAY_INIT_SEC   5   /* thoi gian gateway "ban" khoi tao he thong */
#define WORKER_INIT_SEC    2   /* thoi gian worker gia lap khoi dong */
#define WORKER_EXIT_CODE   7   /* ma exit tuy y de cha kiem tra */

static void handle_sigusr1(int sig)
{
    (void)sig;
    /* Chi dung write() - async-signal-safe - ben trong handler */
    static const char msg[] =
        "[GATEWAY] Worker reported READY signal received\n";
    write(STDOUT_FILENO, msg, sizeof(msg) - 1);
}

int main(void)
{
    /* Dang ky handler TRUOC khi fork(), de dam bao hanh vi signal
     * duoc thiet lap ro rang truoc khi co tien trinh con nao ton tai. */
    if (signal(SIGUSR1, handle_sigusr1) == SIG_ERR) {
        perror("signal(SIGUSR1) failed");
        return EXIT_FAILURE;
    }

    pid_t child_pid = fork();
    if (child_pid < 0) {
        perror("fork failed");
        return EXIT_FAILURE;
    }

    if (child_pid == 0) {
        /* ================= WORKER (tien trinh con) ================= */
        sleep(WORKER_INIT_SEC);

        if (kill(getppid(), SIGUSR1) != 0) {
            perror("kill(SIGUSR1) failed");
            _exit(EXIT_FAILURE);
        }

        printf("[WORKER] Sent READY signal to gateway\n");
        fflush(stdout);

        _exit(WORKER_EXIT_CODE);
        /* khong dung exit() thuong o tien trinh con sau fork() khi co
         * the co stdio buffer dang cho tu tien trinh cha, dung _exit()
         * de tranh flush trung buffer / goi atexit() 2 lan. O day da
         * fflush() thu cong nen an toan, nhung _exit() van la thoi
         * quen dung trong tien trinh con. */
    }

    /* ================= GATEWAY (tien trinh cha) ================= */
    printf("[GATEWAY] Worker PID = %d\n", (int)child_pid);
    fflush(stdout);

    sigset_t block_set, old_set;
    sigemptyset(&block_set);
    sigaddset(&block_set, SIGUSR1);

    /* Block SIGUSR1: neu worker gui signal trong luc nay, signal se
     * o trang thai PENDING, khong bi mat va khong duoc xu ly ngay. */
    if (sigprocmask(SIG_BLOCK, &block_set, &old_set) != 0) {
        perror("sigprocmask(SIG_BLOCK) failed");
        return EXIT_FAILURE;
    }

    sleep(GATEWAY_INIT_SEC);

    /* Unblock: neu SIGUSR1 dang pending, kernel se goi handler ngay
     * tai thoi diem nay, truoc khi sigprocmask() tra ve. */
    if (sigprocmask(SIG_SETMASK, &old_set, NULL) != 0) {
        perror("sigprocmask(restore) failed");
        return EXIT_FAILURE;
    }

    int status;
    if (wait(&status) < 0) {
        perror("wait failed");
        return EXIT_FAILURE;
    }

    if (WIFEXITED(status)) {
        printf("[GATEWAY] Worker exited with code %d\n", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        printf("[GATEWAY] Worker was killed by signal %d\n", WTERMSIG(status));
    } else {
        printf("[GATEWAY] Worker terminated abnormally\n");
    }

    return EXIT_SUCCESS;
}