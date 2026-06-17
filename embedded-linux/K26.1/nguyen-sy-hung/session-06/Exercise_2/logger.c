#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>


#define LOG_ERR     "<3>"
#define LOG_WARNING "<4>"
#define LOG_INFO    "<6>"


int main(void)
{
    int cycle = 0;
    time_t start_time;

    /* Disable buffering */
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);


    start_time = time(NULL);


    fprintf(stderr, LOG_INFO "Logger service started\n");


    while (1)
    {
        fprintf(stderr,
                LOG_INFO "Service running normally, cycle %d\n",
                cycle);


        fprintf(stderr,
                LOG_WARNING "Memory usage high: %d%%\n",
                80 + rand() % 15);


        fprintf(stderr,
                LOG_ERR "Failed to connect to database, retry %d\n",
                cycle);


        sleep(2);


        cycle++;


        if ((time(NULL) - start_time) >= 30)
        {
            fprintf(stderr,
                    LOG_ERR "Fatal error detected, aborting service\n");

            abort();
        }
    }


    return 0;
}