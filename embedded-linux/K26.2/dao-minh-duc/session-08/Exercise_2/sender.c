#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define COLLECTOR_IP "127.0.0.1"
#define COLLECTOR_PORT 9000
#define BUFFER_SIZE 128

int read_load_average(double *load1)
{
    FILE *file;
    double load5;
    double load15;
    int running_processes;
    unsigned long last_pid;

    file = fopen("/proc/loadavg", "r");

    if (file == NULL)
    {
        perror("fopen /proc/loadavg");
        return -1;
    }

    if (fscanf(file,
               "%lf %lf %lf %d/%d %lu",
               load1,
               &load5,
               &load15,
               &running_processes,
               &running_processes,
               &last_pid) < 1)
    {
        fclose(file);
        return -1;
    }

    fclose(file);

    return 0;
}

int read_memory(long *mem_total, long *mem_free)
{
    FILE *file;
    char line[128];

    *mem_total = 0;
    *mem_free = 0;

    file = fopen("/proc/meminfo", "r");

    if (file == NULL)
    {
        perror("fopen /proc/meminfo");
        return -1;
    }

    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (sscanf(line, "MemTotal: %ld kB", mem_total) == 1)
        {
            continue;
        }

        if (sscanf(line, "MemFree: %ld kB", mem_free) == 1)
        {
            continue;
        }

        if (*mem_total > 0 && *mem_free > 0)
        {
            break;
        }
    }

    fclose(file);

    if (*mem_total <= 0 || *mem_free < 0)
    {
        return -1;
    }

    return 0;
}

int main(void)
{
    int sensor_fd;
    struct sockaddr_in collector_addr;

    sensor_fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sensor_fd == -1)
    {
        perror("socket");
        return 1;
    }

    memset(&collector_addr, 0, sizeof(collector_addr));

    collector_addr.sin_family = AF_INET;
    collector_addr.sin_port = htons(COLLECTOR_PORT);

    if (inet_pton(AF_INET,
                  COLLECTOR_IP,
                  &collector_addr.sin_addr) != 1)
    {
        perror("inet_pton");
        close(sensor_fd);
        return 1;
    }

    setbuf(stdout, NULL);

    printf("[Sensor] Target collector: %s:%d\n",
           COLLECTOR_IP,
           COLLECTOR_PORT);

    for (int i = 1; i <= 5; i++)
    {
        double load1;
        long mem_total;
        long mem_free;
        double mem_used_pct;
        double temperature;
        char message[BUFFER_SIZE];

        if (read_load_average(&load1) == -1)
        {
            fprintf(stderr, "Failed to read load average\n");
            close(sensor_fd);
            return 1;
        }

        if (read_memory(&mem_total, &mem_free) == -1)
        {
            fprintf(stderr, "Failed to read memory information\n");
            close(sensor_fd);
            return 1;
        }

        temperature = 40.0 + load1 * 10.0;

        mem_used_pct =
            (double)(mem_total - mem_free) /
            mem_total * 100.0;

        snprintf(message,
                 sizeof(message),
                 "id=sensor-01 temp=%.1f mem_used=%.1f%%",
                 temperature,
                 mem_used_pct);

        ssize_t sent;

        sent = sendto(sensor_fd,
                      message,
                      strlen(message),
                      0,
                      (struct sockaddr *)&collector_addr,
                      sizeof(collector_addr));

        if (sent == -1)
        {
            perror("sendto");
            close(sensor_fd);
            return 1;
        }

        printf("[Sent %d/5] %s\n", i, message);

        if (i < 5)
        {
            sleep(2);
        }
    }

    printf("[Sensor] Done.\n");

    close(sensor_fd);

    return 0;
}
