#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>


static volatile sig_atomic_t running = 1;


static void signal_handler(int signo)
{
    if (signo == SIGTERM)
    {
        running = 0;
    }
}


int main(void)
{
    struct sigaction sa;


    setbuf(stdout, NULL);


    sa.sa_handler = signal_handler;
    sa.sa_flags = 0;

    if (sigemptyset(&sa.sa_mask) == -1)
    {
        perror("sigemptyset");
        return EXIT_FAILURE;
    }


    if (sigaction(SIGTERM, &sa, NULL) == -1)
    {
        perror("sigaction");
        return EXIT_FAILURE;
    }


    printf("Monitor service started\n");


    while (running)
    {
        printf("Monitor running\n");
        sleep(1);
    }


    printf("Service shutting down...\n");


    return EXIT_SUCCESS;
}