#include <stdio.h>
#include <stdlib.h>
#include <signal.h> 
#include <unistd.h>
#include <time.h>

volatile sig_atomic_t reading_count = 0;
volatile sig_atomic_t should_exit = 0;

void handle_sigint(int sig) {
    printf("[WARN] Received SIGINT, ignoring...\n");
}

void handle_sigterm(int sig) {
    printf("[INFO] Received SIGTERM, shutting down gracefully...\n");
    should_exit = 1;
}

void handle_sigusr1(int sig) {
    printf("[REPORT] Total readings so far: %d\n", reading_count);
}

int main() {
    // Register signal handlers
    signal(SIGINT, handle_sigint);
    signal(SIGTERM, handle_sigterm);
    signal(SIGUSR1, handle_sigusr1);
    
    // Seed random number generator
    srand(time(NULL));
    
    // Main loop
    while (!should_exit) {
        reading_count++;
        int temperature = 20 + (rand() % 15);  // Random temperature between 20-34
        printf("[INFO] Sensor reading #%d: temperature=%d\n", reading_count, temperature);
        sleep(1);
    }
    
    exit(0);
}