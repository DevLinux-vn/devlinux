#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>

#define LOG_ERR     "<3>"
#define LOG_WARNING "<4>"
#define LOG_INFO    "<6>"

#define CYCLE_DURATION_SEC 2
#define CRASH_TIME_SEC 30
#define MEM_USAGE_BASE 80
#define MEM_USAGE_VARIANCE 15

static void sleep_full_duration(unsigned int seconds)
{
    while (seconds > 0) {
        seconds = sleep(seconds);
    }
}

int main(void)
{
    uint32_t cycle = 1;

    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    srand((unsigned int)(time(NULL) ^ getpid()));

    while (1) {
        if (fprintf(stderr, LOG_INFO "Service running normally, cycle %u\n", cycle) < 0) {
            perror("fprintf");
            return EXIT_FAILURE;
        }

        if (fprintf(stderr, LOG_WARNING "Memory usage high: %d%%\n",
                    MEM_USAGE_BASE + rand() % MEM_USAGE_VARIANCE) < 0) {
            perror("fprintf");
            return EXIT_FAILURE;
        }

        if (fprintf(stderr, LOG_ERR "Failed to connect to database, retry %u\n", cycle) < 0) {
            perror("fprintf");
            return EXIT_FAILURE;
        }

        sleep_full_duration(CYCLE_DURATION_SEC);

        /*
         * Intentional crash after configured runtime
         * to demonstrate systemd/journal behavior.
         */
        if (cycle * CYCLE_DURATION_SEC >= CRASH_TIME_SEC) {
            fprintf(stderr, LOG_ERR "Service crashed intentionally after %d seconds\n", CRASH_TIME_SEC);
            abort();
        }

        cycle++;
    }

    return EXIT_SUCCESS;
}