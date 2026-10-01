/**
 * @file logger.c
 * @brief Service generating logs at different severity levels and crashing via abort().
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define LOG_ERR     "<3>"
#define LOG_WARNING "<4>"
#define LOG_INFO    "<6>"

int main(void)
{
    /* Disable buffering for immediate journal visibility */
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    srand((unsigned int)time(NULL));

    int cycle = 1;
    /* Run for 30 seconds (15 cycles * 2 seconds = 30 seconds) */
    while (cycle <= 15) {
        fprintf(stderr, LOG_INFO    "Service running normally, cycle %d\n", cycle);
        fprintf(stderr, LOG_WARNING "Memory usage high: %d%%\n", 80 + rand() % 15);
        fprintf(stderr, LOG_ERR     "Failed to connect to database, retry %d\n", cycle);

        sleep(2);
        cycle++;
    }

    /* Simulate crash after 30 seconds as specified */
    fprintf(stderr, LOG_ERR "Critical failure occurred. Triggering abort()...\n");
    abort();

    return EXIT_SUCCESS;
}
