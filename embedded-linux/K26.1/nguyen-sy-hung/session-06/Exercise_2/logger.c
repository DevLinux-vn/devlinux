#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>


#define LOG_ERR     "<3>"
#define LOG_WARNING "<4>"
#define LOG_INFO    "<6>"


static void cleanup(void)
{
    fprintf(stderr, LOG_INFO "Cleaning up resources before crash\n");
}


int main(void)
{
    int cycle = 0;
    time_t start_time;
    time_t current_time;


    setbuf(stdout, NULL);
    setbuf(stderr, NULL);


    start_time = time(NULL);

    if (start_time == (time_t)-1)
    {
        fprintf(stderr, LOG_ERR "Failed to get start time\n");
        return EXIT_FAILURE;
    }


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


        current_time = time(NULL);

        if (current_time == (time_t)-1)
        {
            fprintf(stderr,
                    LOG_ERR "Failed to get current time\n");

            return EXIT_FAILURE;
        }


        if ((current_time - start_time) >= 30)
        {
            fprintf(stderr,
                    LOG_ERR "Fatal error detected\n");


            cleanup();


            abort();
        }


        cycle++;
    }


    return EXIT_SUCCESS;
}