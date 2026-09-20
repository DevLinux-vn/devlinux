#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "sensor_shm.h"

static sensor_data_t *g_shm_ptr = (void *)-1;
static int g_shm_id = -1;

static void cleanup_and_exit(int signum)
{
    (void)signum;

    if (g_shm_ptr != (void *)-1) {
        if (shmdt(g_shm_ptr) == -1) {
            perror("[Daemon] shmdt");
        }
    }

    if (g_shm_id != -1) {
        if (shmctl(g_shm_id, IPC_RMID, NULL) == -1) {
            perror("[Daemon] shmctl(IPC_RMID)");
        }
    }

    printf("\n[Daemon] Cleaning up shared memory. Goodbye.\n");
    exit(EXIT_SUCCESS);
}

static int read_cpu_temp(double *out_temp)
{
    FILE *fp = fopen("/proc/loadavg", "r");
    if (fp == NULL) {
        perror("[Daemon] fopen(/proc/loadavg)");
        return -1;
    }

    double load1 = 0.0;
    if (fscanf(fp, "%lf", &load1) != 1) {
        fprintf(stderr, "[Daemon] Failed to parse /proc/loadavg\n");
        fclose(fp);
        return -1;
    }

    fclose(fp);

    *out_temp = 40.0 + load1 * 10.0;
    return 0;
}

static int read_ram_usage(double *out_pct)
{
    FILE *fp = fopen("/proc/meminfo", "r");
    if (fp == NULL) {
        perror("[Daemon] fopen(/proc/meminfo)");
        return -1;
    }

    char line[256];
    long mem_total_kb = -1;
    long mem_free_kb = -1;

    while (fgets(line, sizeof(line), fp) != NULL) {
        if (mem_total_kb == -1 && strncmp(line, "MemTotal:", 9) == 0) {
            sscanf(line + 9, "%ld", &mem_total_kb);
        } else if (mem_free_kb == -1 && strncmp(line, "MemFree:", 8) == 0) {
            sscanf(line + 8, "%ld", &mem_free_kb);
        }

        if (mem_total_kb != -1 && mem_free_kb != -1) {
            break;
        }
    }

    fclose(fp);

    if (mem_total_kb <= 0 || mem_free_kb < 0) {
        fprintf(stderr, "[Daemon] Failed to parse /proc/meminfo\n");
        return -1;
    }

    *out_pct = ((double)(mem_total_kb - mem_free_kb) / (double)mem_total_kb) * 100.0;
    return 0;
}

int main(void)
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = cleanup_and_exit;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("[Daemon] sigaction");
        exit(EXIT_FAILURE);
    }

    g_shm_id = shmget(SHM_KEY, sizeof(sensor_data_t), IPC_CREAT | 0666);
    if (g_shm_id == -1) {
        perror("[Daemon] shmget");
        exit(EXIT_FAILURE);
    }

    g_shm_ptr = (sensor_data_t *)shmat(g_shm_id, NULL, 0);
    if (g_shm_ptr == (void *)-1) {
        perror("[Daemon] shmat");
        shmctl(g_shm_id, IPC_RMID, NULL);
        exit(EXIT_FAILURE);
    }

    printf("[Daemon] Shared memory created. Key=0x%X\n", SHM_KEY);

    while (1) {
        double cpu_temp = 0.0;
        double ram_used_pct = 0.0;

        if (read_cpu_temp(&cpu_temp) == -1) {
            cleanup_and_exit(0);
        }

        if (read_ram_usage(&ram_used_pct) == -1) {
            cleanup_and_exit(0);
        }

        g_shm_ptr->timestamp = time(NULL);
        g_shm_ptr->cpu_temp = cpu_temp;
        g_shm_ptr->ram_used_pct = ram_used_pct;

        printf("[Daemon] Written: temp=%.2f ram=%.2f%%\n", cpu_temp, ram_used_pct);
        fflush(stdout);

        sleep(2);
    }

    return 0;
}
