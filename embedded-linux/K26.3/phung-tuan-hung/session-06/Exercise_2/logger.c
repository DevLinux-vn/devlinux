#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define LOG_ERR     "<3>"
#define LOG_WARNING "<4>"
#define LOG_INFO    "<6>"

int main(void)
{
    int cycle = 0;
    time_t start;

    setbuf(stdout, NULL);
    setbuf(stderr, NULL);
    srand((unsigned)time(NULL));
    start = time(NULL);

    for (;;) {
        cycle++;
        fprintf(stderr, LOG_INFO    "Service running normally, cycle %d\n", cycle);
        fprintf(stderr, LOG_WARNING "Memory usage high: %d%%\n", 80 + rand() % 15);
        fprintf(stderr, LOG_ERR     "Failed to connect to database, retry %d\n", cycle);

        sleep(2);

        if (time(NULL) - start >= 30) {
            fprintf(stderr, LOG_ERR "Fatal error, aborting after 30 seconds\n");
            abort(); /* simulate a crash (SIGABRT) */
        }
    }
    return 0;
}
