#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

volatile int running = 1;

void sigterm_handler(int sig) {
    printf("Service shutting down...\n");
    running = 0;
}

int main() {
    setbuf(stdout, NULL);
    
    signal(SIGTERM, sigterm_handler);
    
    int count = 0;
    while (running) {
        time_t now = time(NULL);
        struct tm *tm_info = localtime(&now);
        char time_str[64];
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);
        
        printf("[%s] Monitor running - cycle %d\n", time_str, count++);
        sleep(1);
    }
    
    return 0;
}
