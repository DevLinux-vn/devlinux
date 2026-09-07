#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define LOG_ERR "<3>"
#define LOG_WARNING "<4>"
#define LOG_INFO "<6>"

int main()
{
    int totalSec = 0;
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);
    while (totalSec < 30)
    {
        sleep(2);
        totalSec += 2;
        int cycle = rand();
        fprintf(stdout, LOG_INFO "Service running normally, cycle %d\n", cycle);
        fprintf(stderr, LOG_WARNING "Memory usage high: %d%%\n", 80 + rand() % 15);
        fprintf(stderr, LOG_ERR "Failed to connect to database, retry %d\n", cycle);
    }

    abort();
    return 0;
}