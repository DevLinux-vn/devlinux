#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

#include <sys/ipc.h>
#include <sys/shm.h>

#include "sensor_shm.h"

#define LOADAVG_FILE           "/proc/loadavg"
#define MEMINFO_FILE           "/proc/meminfo"

#define LOADAVG_SIZE           64U
#define MEMINFO_LINE_SIZE      128U

#define UPDATE_INTERVAL_SEC    2U

static int g_shmid = -1;
static sensor_data_t *g_sensor_data = NULL;

/*------------------------------------------------------------------*/

static void cleanup(void)
{
    if (g_sensor_data != NULL)
    {
        if (shmdt(g_sensor_data) == -1)
        {
            perror("shmdt");
        }

        g_sensor_data = NULL;
    }

    if (g_shmid != -1)
    {
        if (shmctl(g_shmid, IPC_RMID, NULL) == -1)
        {
            perror("shmctl");
        }

        g_shmid = -1;
    }
}

/*------------------------------------------------------------------*/

static void signal_handler(int signo)
{
    (void)signo;

    printf("\n[Daemon] Cleaning up shared memory. Goodbye.\n");

    cleanup();

    exit(EXIT_SUCCESS);
}

/*------------------------------------------------------------------*/

static int create_shared_memory(void)
{
    g_shmid = shmget(SHM_KEY,
                     sizeof(sensor_data_t),
                     IPC_CREAT | SHM_PERMISSION);

    if (g_shmid == -1)
    {
        perror("shmget");
        return -1;
    }

    g_sensor_data = (sensor_data_t *)shmat(g_shmid, NULL, 0);

    if (g_sensor_data == (void *)-1)
    {
        perror("shmat");
        g_sensor_data = NULL;
        return -1;
    }

    return 0;
}

/*------------------------------------------------------------------*/

static int read_cpu_temperature(double *temperature)
{
    FILE *fp;

    double load_avg;

    fp = fopen(LOADAVG_FILE, "r");
    if (fp == NULL)
    {
        perror("fopen");
        return -1;
    }

    if (fscanf(fp, "%lf", &load_avg) != 1)
    {
        fprintf(stderr, "Failed to read load average\n");
        fclose(fp);
        return -1;
    }

    fclose(fp);

    *temperature = 40.0 + (load_avg * 10.0);

    return 0;
}

/*------------------------------------------------------------------*/

static int read_ram_usage(double *ram_used_pct)
{
    FILE *fp;

    char line[MEMINFO_LINE_SIZE];

    long mem_total = 0;
    long mem_free = 0;

    fp = fopen(MEMINFO_FILE, "r");
    if (fp == NULL)
    {
        perror("fopen");
        return -1;
    }

    while (fgets(line, sizeof(line), fp) != NULL)
    {
        if (sscanf(line, "MemTotal: %ld", &mem_total) == 1)
        {
            continue;
        }

        if (sscanf(line, "MemFree: %ld", &mem_free) == 1)
        {
            continue;
        }

        if ((mem_total != 0) && (mem_free != 0))
        {
            break;
        }
    }

    fclose(fp);

    if (mem_total == 0)
    {
        fprintf(stderr, "Invalid MemTotal\n");
        return -1;
    }

    *ram_used_pct =
        ((double)(mem_total - mem_free) /
         (double)mem_total) * 100.0;

    return 0;
}

/*------------------------------------------------------------------*/

static int update_sensor_data(void)
{
    double cpu_temp;
    double ram_used_pct;

    if (read_cpu_temperature(&cpu_temp) == -1)
    {
        return -1;
    }

    if (read_ram_usage(&ram_used_pct) == -1)
    {
        return -1;
    }

    g_sensor_data->timestamp = time(NULL);
    g_sensor_data->cpu_temp = cpu_temp;
    g_sensor_data->ram_used_pct = ram_used_pct;

    printf("[Daemon] Written: temp=%.2f C ram=%.2f%%\n",
           cpu_temp,
           ram_used_pct);

    return 0;
}

/*------------------------------------------------------------------*/

int main(void)
{
    if (signal(SIGINT, signal_handler) == SIG_ERR)
    {
        perror("signal");
        return EXIT_FAILURE;
    }

    if (create_shared_memory() == -1)
    {
        return EXIT_FAILURE;
    }

    printf("[Daemon] Shared memory created. Key=0x%X\n",
           SHM_KEY);

    while (1)
    {
        if (update_sensor_data() == -1)
        {
            break;
        }

        sleep(UPDATE_INTERVAL_SEC);
    }

    cleanup();

    return EXIT_SUCCESS;
}