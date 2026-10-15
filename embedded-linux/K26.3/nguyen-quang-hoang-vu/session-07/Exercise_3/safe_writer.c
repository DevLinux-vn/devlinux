#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t interrupted = 0;

void handle_sigint(int sig)
{
    (void)sig;

    interrupted = 1;

    printf(
        "\n[INFO] SIGINT received\n");
}

int main(void)
{
    sigset_t block_set;
    sigset_t old_set;

    signal(SIGINT,
           handle_sigint);

    sigemptyset(
        &block_set);

    sigaddset(
        &block_set,
        SIGINT);

    printf(
        "[SAFE] Starting transaction processing\n");

    for (int i = 1;
         i <= 5;
         i++)
    {
        printf(
            "[SAFE] Writing transaction #%d ...\n",
            i);

        sigprocmask(
            SIG_BLOCK,
            &block_set,
            &old_set);

        sleep(3);

        printf(
            "[SAFE] Transaction #%d committed\n",
            i);

        sigprocmask(
            SIG_SETMASK,
            &old_set,
            NULL);

        sleep(1);

        if (interrupted)
        {
            printf(
                "[SAFE] Graceful shutdown\n");

            break;
        }
    }

    return 0;
}
