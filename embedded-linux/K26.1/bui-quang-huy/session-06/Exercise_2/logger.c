#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define LOG_ERR     "<3>"
#define LOG_WARNING "<4>"
#define LOG_INFO    "<6>"

int main(void) {
    /* Unbuffer both standard streams as requested by specification */
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    int cycle = 1;
    
    /* 30 seconds total runtime with 2-second interval implies exactly 15 cycles */
    while (cycle <= 15) {
        fprintf(stderr, LOG_INFO "Service running normally, cycle %d\n", cycle);
        
        /* Simulated dynamic warning metrics */
        fprintf(stderr, LOG_WARNING "Memory usage high: %d%%\n", 80 + (cycle % 15));
        
        fprintf(stderr, LOG_ERR "Failed to connect to database, retry %d\n", cycle);
        
        sleep(2);
        cycle++;
    }

    /* Induce an abnormal termination signal (SIGABRT) to capture crash profile */
    abort();

    return EXIT_SUCCESS; /* Unreachable code, kept for compiler static analysis integrity */
}