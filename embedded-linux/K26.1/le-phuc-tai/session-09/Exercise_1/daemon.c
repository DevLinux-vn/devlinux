/**
 * @file daemon.c
 * @brief Background telemetric service fetching system statistics into SystemV SHM.
 * @date 2026-07-04
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/shm.h>
#include <errno.h>
#include "sensor_shm.h"

#define INTERVAL_SEC      2     /* Periodic sleep cycle duration in seconds */
#define DEFAULT_FALLBACK  0.0   /* Safe default value for failed metrics */
#define TEMP_BASE_CALC    40.0  /* Static offset for simulated temperature */
#define TEMP_MULTIPLIER   10.0  /* Scaling factor for load-based temperature */

volatile sig_atomic_t g_keep_running = 1;

static void handle_sigint(int signum) {
    (void)signum;
    g_keep_running = 0;
}

static void read_system_metrics(double *temp_out, double *ram_out) {
    double load1 = 0.0;
    
    /* Defensive check: Validate file existence and reading permissions */
    FILE *f_cpu = fopen("/proc/loadavg", "r");
    if (f_cpu == NULL) {
        fprintf(stderr, "[ERROR] /proc/loadavg missing or inaccessible\n");
        *temp_out = DEFAULT_FALLBACK;
    } else {
        if (fscanf(f_cpu, "%lf", &load1) != 1) {
            load1 = 0.0;
        }
        fclose(f_cpu);
        *temp_out = TEMP_BASE_CALC + (load1 * TEMP_MULTIPLIER);
    }

    long mem_total = 1, mem_free = 0;
    char line[128];
    FILE *f_mem = fopen("/proc/meminfo", "r");
    if (f_mem == NULL) {
        fprintf(stderr, "[ERROR] /proc/meminfo missing or inaccessible\n");
        *ram_out = DEFAULT_FALLBACK;
        return;
    }
    
    while (fgets(line, sizeof(line), f_mem)) {
        if (strncmp(line, "MemTotal:", 9) == 0) {
            sscanf(line, "MemTotal: %ld", &mem_total);
        } else if (strncmp(line, "MemFree:", 8) == 0) {
            sscanf(line, "MemFree: %ld", &mem_free);
        }
    }
    fclose(f_mem);
    
    if (mem_total <= 0) mem_total = 1; /* Mitigate division by zero */
    *ram_out = (double)(mem_total - mem_free) / (double)mem_total * 100.0;
}

int main(void) {
    setbuf(stdout, NULL);

    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGINT, &sa, NULL) < 0) {
        perror("CRITICAL: Failed to register SIGINT");
        return EXIT_FAILURE;
    }

    int shmid = shmget(SHM_KEY, sizeof(sensor_data_t), IPC_CREAT | 0666);
    if (shmid < 0) {
        if (errno == EACCES) {
            fprintf(stderr, "CRITICAL: Insufficient privileges for SHM Key 0x%x.\n", SHM_KEY);
        } else if (errno == EEXIST) {
            fprintf(stderr, "CRITICAL: Key collision detected for SHM Key 0x%x.\n", SHM_KEY);
        } else if (errno == ENOMEM) {
            fprintf(stderr, "CRITICAL: Kernel out of memory during SHM allocation.\n");
        } else {
            perror("CRITICAL: shmget initialization failed");
        }
        return EXIT_FAILURE;
    }
    printf("[Daemon] Shared memory created. Key=0x%x\n", SHM_KEY);

    sensor_data_t *shm_ptr = (sensor_data_t *)shmat(shmid, NULL, 0);
    if (shm_ptr == (void *)-1) {
        perror("CRITICAL: shmat memory binding failed");
        return EXIT_FAILURE;
    }

    while (g_keep_running) {
        double temp = DEFAULT_FALLBACK, ram_pct = DEFAULT_FALLBACK;
        read_system_metrics(&temp, &ram_pct);

        /* Robust check: Verify return value of time() system API */
        time_t current_time = time(NULL);
        if (current_time == (time_t)-1) {
            perror("ERROR: System time fetch failed");
            shm_ptr->timestamp = 0; 
        } else {
            shm_ptr->timestamp = current_time;
        }

        shm_ptr->cpu_temp = temp;
        shm_ptr->ram_used_pct = ram_pct;

        printf("[Daemon] Written: temp=%.2f C ram=%.2f%%\n", temp, ram_pct);
        sleep(INTERVAL_SEC);
    }

    printf("\n[Daemon] Disconnecting shared structures and removing RMID...\n");
    if (shmdt(shm_ptr) < 0) perror("ERROR: shmdt failed");
    if (shmctl(shmid, IPC_RMID, NULL) < 0) perror("ERROR: shmctl IPC_RMID failed");

    return EXIT_SUCCESS;
}