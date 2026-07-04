#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include "sensor_shm.h"

int shm_id = -1;
sensor_data_t *sensor_data = NULL;

void cleanup(int sig) {
    if (sensor_data != NULL) {
        shmdt(sensor_data);
    }
    if (shm_id != -1) {
        shmctl(shm_id, IPC_RMID, NULL);
    }
    printf("\n[Daemon] Cleaning up shared memory. Goodbye.\n");
    exit(0);
}

void read_cpu_load(double *load1) {
    FILE *f = fopen("/proc/loadavg", "r");
    if (!f) {
        *load1 = 0.0;
        return;
    }
    fscanf(f, "%lf", load1);
    fclose(f);
}

void read_mem_info(double *mem_used_pct) {
    FILE *f = fopen("/proc/meminfo", "r");
    if (!f) {
        *mem_used_pct = 0.0;
        return;
    }
    
    long mem_total = 0, mem_free = 0;
    char line[128];
    
    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, "MemTotal:", 9) == 0) {
            sscanf(line, "MemTotal: %ld", &mem_total);
        } else if (strncmp(line, "MemFree:", 8) == 0) {
            sscanf(line, "MemFree: %ld", &mem_free);
        }
    }
    fclose(f);
    
    if (mem_total > 0) {
        *mem_used_pct = (double)(mem_total - mem_free) / mem_total * 100.0;
    } else {
        *mem_used_pct = 0.0;
    }
}

int main() {
    signal(SIGINT, cleanup);
    
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
        exit(1);
    }
    
    printf("[Daemon] Shared memory created. Key=0x%x\n", SHM_KEY);
    
    // Write data every 2 seconds
    while (1) {
        double load1, mem_used_pct;
        read_cpu_load(&load1);
        read_mem_info(&mem_used_pct);
        
        sensor_data->timestamp = time(NULL);
        sensor_data->cpu_temp = 40.0 + load1 * 10.0;
        sensor_data->ram_used_pct = mem_used_pct;
        
        printf("[Daemon] Written: temp=%.2f ram=%.2f%%\n", sensor_data->cpu_temp, sensor_data->ram_used_pct);
        
        sleep(2);
    }
    
    return 0;
}
