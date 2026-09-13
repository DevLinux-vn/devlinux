#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

/* Constants to avoid magic numbers */
#define SLEEP_INTERVAL_SEC 1
#define TEMP_BASE 25
#define TEMP_VARIANCE 10

typedef struct
{
    sig_atomic_t s_int;
    sig_atomic_t s_tern;
    sig_atomic_t s_user1;
} signal_flag;

/*Counter variable for main loop and signal handler */
volatile sig_atomic_t reading_count = 0;

volatile signal_flag flag = {
    .s_int = 0,
    .s_tern = 0,
    .s_user1 = 0};

/* Handler for SIGINT (Ctrl+C) */
void handle_sigint(int sig)
{
    (void)sig; // Bỏ qua cảnh báo unused parameter
    flag.s_int = 1;
}

/* Handler for SIGTERM */
void handle_sigterm(int sig)
{
    (void)sig;
    flag.s_tern = 1;
}

/* Handler for SIGUSR1 */
void handle_sigusr1(int sig)
{
    (void)sig;
    flag.s_user1 = 1;
}

int main(void)
{
    // 1. Register signal handlers
    if (signal(SIGINT, handle_sigint) == SIG_ERR)
    {
        perror("Failed to register SIGINT");
        return EXIT_FAILURE;
    }
    if (signal(SIGTERM, handle_sigterm) == SIG_ERR)
    {
        perror("Failed to register SIGTERM");
        return EXIT_FAILURE;
    }
    if (signal(SIGUSR1, handle_sigusr1) == SIG_ERR)
    {
        perror("Failed to register SIGUSR1");
        return EXIT_FAILURE;
    }

    // Init random value
    srand((unsigned int)time(NULL));

    printf("Sensor daemon is running. PID: %d\n", getpid());
    printf("Send signals from another terminal to test:\n");
    printf("  kill -INT %d  (or press Ctrl+C)\n", getpid());
    printf("  kill -USR1 %d\n", getpid());
    printf("  kill -TERM %d\n\n", getpid());

    // 2. Main loop
    while (1)
    {
        if (flag.s_int == 1)
        {
            printf("\n[WARN] Received SIGINT, ignoring...\n");
            fflush(stdout);
            flag.s_int = 0;
        }

        if (flag.s_tern == 1)
        {
            printf("\n[INFO] Received SIGTERM, shutting down gracefully...\n");
            flag.s_tern = 0;
            exit(EXIT_SUCCESS);
        }

        if (flag.s_user1 == 1)
        {
            printf("\n[REPORT] Total readings so far: %d\n", (int)reading_count);
            fflush(stdout);
            flag.s_user1 = 0;
        }

        int temp = TEMP_BASE + (rand() % TEMP_VARIANCE);
        reading_count++;

        printf("[INFO] Sensor reading #%d: temperature=%d\n", (int)reading_count, temp);
        fflush(stdout);
        sleep(SLEEP_INTERVAL_SEC);
    }

    return EXIT_SUCCESS;
}
