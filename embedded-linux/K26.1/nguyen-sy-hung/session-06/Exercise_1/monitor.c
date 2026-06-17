#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>


static volatile sig_atomic_t running = 1;


/*
 * Handle SIGTERM from systemd
 */
static void signal_handler(int sig)
{
    if (sig == SIGTERM)
    {
        printf("Service shutting down...\n");
        running = 0;
    }
}


int main(void)
{
    int counter = 0;

    /*
     * Disable stdout buffering
     * so logs appear immediately in journal
     */
    setbuf(stdout, NULL);

    /*
     * Register SIGTERM handler
     */
    signal(SIGTERM, signal_handler);


    printf("Monitor service started\n");


    while (running)
    {
        printf("Monitor running: %d\n", counter++);
        sleep(1);
    }


    return 0;
}