#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define LOG_ERR     "<3>"
#define LOG_WARNING "<4>"
#define LOG_INFO    "<6>"

int main() {
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);
    
    time_t start_time = time(NULL);
    int cycle = 0;
    
    while (1) {
        time_t now = time(NULL);
        int elapsed = (int)(now - start_time);
        
        // Every 2 seconds, write 3 log lines
        if (elapsed % 2 == 0 && cycle * 2 <= elapsed) {
            cycle++;
            
            fprintf(stderr, LOG_INFO    "Service running normally, cycle %d\n", cycle);
            fprintf(stderr, LOG_WARNING "Memory usage high: %d%%\n", 80 + rand() % 15);
            fprintf(stderr, LOG_ERR     "Failed to connect to database, retry %d\n", cycle);
        }
        
        // Crash after 30 seconds
        if (elapsed >= 30) {
            fprintf(stderr, LOG_ERR "Critical error detected, aborting service\n");
            abort();
        }
        
        sleep(1);
    }
    
    return 0;
}
