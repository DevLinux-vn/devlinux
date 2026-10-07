#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

// Flag to control the infinite loop
volatile sig_atomic_t keep_running = 1;

// Signal handler for SIGTERM when systemd stops the service[cite: 9]
void sigterm_handler(int signum) {
    (void)signum; // Ignore unused parameter warning
    keep_running = 0; // Change flag state to exit the loop
}

int main() {
    // Disable stdout buffering so log lines appear in the journal immediately[cite: 9]
    setbuf(stdout, NULL);
    
    // Register the SIGTERM signal handler
    signal(SIGTERM, sigterm_handler);
    
    // Infinite loop[cite: 9]
    while (keep_running) {
        printf("Monitor service is running...\n");
        sleep(1); // Print log every 1 second[cite: 9]
    }
    
    // Print message when the service is stopped properly[cite: 9]
    printf("Service shutting down...\n");
    
    return 0;
}