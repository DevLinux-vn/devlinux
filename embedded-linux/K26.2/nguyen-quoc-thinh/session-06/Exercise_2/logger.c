#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define LOG_ERR     "<3>"
#define LOG_WARNING "<4>"
#define LOG_INFO    "<6>"

int main(void) {
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    srand((unsigned int)time(NULL));

    time_t start = time(NULL);
    int cycle = 0;

    while (1) {
        fprintf(stderr, LOG_INFO    "Service running normally, cycle %d\n", cycle);
        fprintf(stderr, LOG_WARNING "Memory usage high: %d%%\n", 80 + rand() % 15);
        fprintf(stderr, LOG_ERR     "Failed to connect to database, retry %d\n", cycle);

        cycle++;

        if (difftime(time(NULL), start) >= 30) {
            fprintf(stderr, LOG_ERR "Fatal error, service crashing now\n");
            abort();
        }

        sleep(2);
    }

    return 0;
}
