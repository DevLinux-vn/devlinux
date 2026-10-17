#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

/* Systemd native log severity prefixes definition */
#define LOG_ERR     "<3>"
#define LOG_WARNING "<4>"
#define LOG_INFO    "<6>"

int main(void) {
    /* CRITICAL: Disable buffering on standard output and error streams */
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    /* Seed the random generator for dynamic memory log simulation */
    srand((unsigned int)time(NULL));

    printf("[LOGGER] Advanced Diagnostic Service Started.\n");

    int cycle = 1;
    int max_cycles = 15; /* 15 cycles * 2s = 30 seconds lifetime boundary */

    while (cycle <= max_cycles) {
        /* Standard informational track log */
        fprintf(stderr, LOG_INFO "Service running normally, cycle %d\n", cycle);
        
        /* Simulating warning criteria */
        fprintf(stderr, LOG_WARNING "Memory usage high: %d%%\n", 80 + rand() % 15);
        
        /* Simulating failure event criteria */
        fprintf(stderr, LOG_ERR "Failed to connect to database, retry %d\n", cycle);

        cycle++;
        sleep(2);
    }

    /* System instruction condition: Simulate forced system crash after 30 seconds */
    fprintf(stderr, LOG_ERR "[LOGGER] Critical internal timeout encountered. Triggering abort()...\n");
    abort();

    return EXIT_SUCCESS; /* Unreachable */
}