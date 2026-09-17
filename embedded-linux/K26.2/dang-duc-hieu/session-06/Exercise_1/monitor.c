#include <signal.h>
#include <stdio.h>
#include <unistd.h>

volatile sig_atomic_t isTerminated = 0;

void term_handler(int sig)
{
    isTerminated = 1;
}

int main()
{
    setbuf(stdout, NULL);
    signal(SIGTERM, term_handler);
    while (!isTerminated)
    {
        fprintf(stdout, "This is the output \n");
        sleep(1);
    }
    fprintf(stdout, "Service shutting down...");

    return 0;
}