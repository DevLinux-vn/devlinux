#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>


#define SENSOR_INTERVAL_SEC   1
#define TEMP_START            20.0
#define TEMP_STEP             0.1
#define REPORT_MSG_BUF_SIZE   64


static volatile sig_atomic_t reading_count = 0;

/*
 * Cac ham duoi day CHI duoc goi cac ham "async-signal-safe" (write(),
 * _exit(), snprintf() an toan trong hau het he thong POSIX hien dai).
 * Tuyet doi khong goi printf()/malloc() ben trong signal handler that su.
 */

static void handle_sigint(int sig)
{
    (void)sig; /* tham so bat buoc theo prototype cua signal(), khong dung den */
    static const char msg[] = "[WARN] Received SIGINT, ignoring...\n";
    write(STDOUT_FILENO, msg, sizeof(msg) - 1);
}

static void handle_sigterm(int sig)
{
    (void)sig;
    static const char msg[] =
        "[INFO] Received SIGTERM, shutting down gracefully...\n";
    write(STDOUT_FILENO, msg, sizeof(msg) - 1);
    /* Dung _exit() (khong phai exit()) trong signal handler de tranh
     * goi lai cac ham cleanup/atexit khong an toan voi signal. */
    _exit(EXIT_SUCCESS);
}

static void handle_sigusr1(int sig)
{
    (void)sig;
    char buf[REPORT_MSG_BUF_SIZE];

    /* snprintf vao buffer tren stack roi write() ra, khong goi printf() truc tiep */
    int len = snprintf(buf, sizeof(buf),
                        "[REPORT] Total readings so far: %d\n",
                        (int)reading_count);
    if (len > 0) {
        write(STDOUT_FILENO, buf, (size_t)len);
    }
}

int main(void)
{
    /* Dang ky handler cho ca 3 signal, kiem tra loi tra ve tu signal() */
    if (signal(SIGINT, handle_sigint) == SIG_ERR) {
        perror("signal(SIGINT) failed");
        return EXIT_FAILURE;
    }
    if (signal(SIGTERM, handle_sigterm) == SIG_ERR) {
        perror("signal(SIGTERM) failed");
        return EXIT_FAILURE;
    }
    if (signal(SIGUSR1, handle_sigusr1) == SIG_ERR) {
        perror("signal(SIGUSR1) failed");
        return EXIT_FAILURE;
    }

    printf("[INFO] Sensor daemon started, PID=%d\n", (int)getpid());
    fflush(stdout);

    double temperature = TEMP_START;

    while (1) {
        printf("[INFO] Sensor reading #%d: temperature=%.2f\n",
               (int)reading_count, temperature);
        fflush(stdout); /* stdout mac dinh la line-buffered khi ra terminal,
                          * nhung fflush() cho chac chan, tranh mat log khi
                          * bi redirect vao file (block-buffered). */

        reading_count++;
        temperature += TEMP_STEP;

        sleep(SENSOR_INTERVAL_SEC);
    }

    return EXIT_SUCCESS; /* khong bao gio toi day */
}