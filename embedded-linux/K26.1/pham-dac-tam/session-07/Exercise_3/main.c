#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int main() {
    sigset_t block_set, old_set;
    
    // Initialize signal set for SIGINT
    sigemptyset(&block_set);
    sigaddset(&block_set, SIGINT);
    
    // Loop 5 times
    for (int i = 1; i <= 5; i++) {
        // === GIAI ĐOẠN 1: VÙNG AN TOÀN (CRITICAL SECTION) ===
        // Block SIGINT and save old mask
        sigprocmask(SIG_BLOCK, &block_set, &old_set);
        
        printf("[SAFE] Writing transaction #%d ...\n", i);
        sleep(3);  // Simulate writing transaction
        printf("[SAFE] Transaction #%d committed.\n", i);
        
        // Restore old mask
        sigprocmask(SIG_SETMASK, &old_set, NULL);
        
        // === GIAI ĐOẠN 2: VÙNG NGHỈ (IDLE) ===
        printf("[IDLE] Waiting for next transaction...\n");
        sleep(3);  // In this period, SIGINT is NOT blocked, can be interrupted
    }
    
    exit(0);
}