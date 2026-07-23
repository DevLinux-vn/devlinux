#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include "sensor_shm.h"

int shm_id = -1;
sensor_data_t *sensor_data = NULL;
volatile sig_atomic_t should_exit = 0;

/* Async-signal-safe signal handler - only set flag */
void signal_handler(int sig) {
    (void)sig;
    should_exit = 1;
}

void read_cpu_load(double *load1) {
    *load1 = 0.0;
    FILE *f = fopen("/proc/loadavg", "r");
    if (!f) {
        fprintf(stderr, "[Daemon] Warning: cannot read /proc/loadavg: %s\n", strerror(errno));
        return;
    }
    
    int ret = fscanf(f, "%lf", load1);
    if (ret != 1) {
        fprintf(stderr, "[Daemon] Warning: fscanf /proc/loadavg failed\n");
        *load1 = 0.0;
    }
    
    if (fclose(f) == EOF) {
        fprintf(stderr, "[Daemon] Warning: fclose /proc/loadavg failed\n");
    }
}

void read_mem_info(double *mem_used_pct) {
    *mem_used_pct = 0.0;
    FILE *f = fopen("/proc/meminfo", "r");
    if (!f) {
        fprintf(stderr, "[Daemon] Warning: cannot read /proc/meminfo: %s\n", strerror(errno));
        return;
    }
    
    long mem_total = 0, mem_free = 0;
    char line[128];
    
    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, "MemTotal:", 9) == 0) {
            if (sscanf(line, "MemTotal: %ld", &mem_total) != 1) {
                fprintf(stderr, "[Daemon] Warning: sscanf MemTotal failed\n");
            }
        } else if (strncmp(line, "MemFree:", 8) == 0) {
            if (sscanf(line, "MemFree: %ld", &mem_free) != 1) {
                fprintf(stderr, "[Daemon] Warning: sscanf MemFree failed\n");
            }
        }
    }
    
    if (ferror(f)) {
        fprintf(stderr, "[Daemon] Warning: fgets error on /proc/meminfo\n");
    }
    
    if (fclose(f) == EOF) {
        fprintf(stderr, "[Daemon] Warning: fclose /proc/meminfo failed\n");
    }
    
    if (mem_total > 0) {
        *mem_used_pct = (double)(mem_total - mem_free) / mem_total * 100.0;
    }
}

int main() {
    // Register signal handler and check return value
    if (signal(SIGINT, signal_handler) == SIG_ERR) {
        perror("signal");
        exit(1);
    }
    
    // Create shared memory
    shm_id = shmget(SHM_KEY, sizeof(sensor_data_t), IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("shmget");
        exit(1);
    }
    
    // Attach to shared memory
    sensor_data = (sensor_data_t *)shmat(shm_id, NULL, 0);
    if (sensor_data == (void *)-1) {
        perror("shmat");
        shmctl(shm_id, IPC_RMID, NULL);
        exit(1);
    }
    
    printf("[Daemon] Shared memory created. Key=0x%x\n", SHM_KEY);
    fflush(stdout);
    
    // Write data every 2 seconds
    while (!should_exit) {
        double load1, mem_used_pct;
        read_cpu_load(&load1);
        read_mem_info(&mem_used_pct);
        
        sensor_data->timestamp = time(NULL);
        sensor_data->cpu_temp = 40.0 + load1 * 10.0;
        sensor_data->ram_used_pct = mem_used_pct;
        
        printf("[Daemon] Written: temp=%.2f ram=%.2f%%\n", sensor_data->cpu_temp, sensor_data->ram_used_pct);
        fflush(stdout);
        
        sleep(2);
    }
    
    /* Cleanup - only called from main, not from signal handler */
    printf("\n[Daemon] Received signal. Cleaning up shared memory...\n");
    fflush(stdout);
    
    if (sensor_data != NULL) {
        if (shmdt(sensor_data) == -1) {
            perror("shmdt");
        }
    }
    
    if (shm_id != -1) {
        if (shmctl(shm_id, IPC_RMID, NULL) == -1) {
            perror("shmctl");
        }
    }
    
    printf("[Daemon] Cleanup done. Goodbye.\n");
    fflush(stdout);
    
    return 0;
}
