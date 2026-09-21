/*SESSION 8 - Exercise 2*/
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define COLLECTOR_IP   "127.0.0.1"
#define COLLECTOR_PORT 9000
#define BUFFER_SIZE    128
#define NUM_SAMPLES    5
#define SENSOR_ID      "sensor-01"

static int read_load1(double *load1)
{
    FILE *f = fopen("/proc/loadavg", "r");
    if (f == NULL) {
        perror("fopen /proc/loadavg");
        return -1;
    }
    if (fscanf(f, "%lf", load1) != 1) {
        fclose(f);
        fprintf(stderr, "Failed to parse /proc/loadavg\n");
        return -1;
    }
    fclose(f);
    return 0;
}

static int read_meminfo(long *mem_total, long *mem_free)
{
    FILE *f = fopen("/proc/meminfo", "r");
    char line[128];

    *mem_total = -1;
    *mem_free = -1;

    if (f == NULL) {
        perror("fopen /proc/meminfo");
        return -1;
    }

    while (fgets(line, sizeof(line), f) != NULL) {
        if (strncmp(line, "MemTotal:", 9) == 0) {
            sscanf(line + 9, "%ld", mem_total);
        } else if (strncmp(line, "MemFree:", 8) == 0) {
            sscanf(line + 8, "%ld", mem_free);
        }
        if (*mem_total != -1 && *mem_free != -1) {
            break;
        }
    }
    fclose(f);

    if (*mem_total == -1 || *mem_free == -1) {
        fprintf(stderr, "Failed to parse /proc/meminfo\n");
        return -1;
    }
    return 0;
}

int main(void)
{
    struct sockaddr_in collector_addr;

    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&collector_addr, 0, sizeof(collector_addr));
    collector_addr.sin_family = AF_INET;
    collector_addr.sin_port = htons(COLLECTOR_PORT);
    if (inet_pton(AF_INET, COLLECTOR_IP, &collector_addr.sin_addr) != 1) {
        fprintf(stderr, "inet_pton failed for %s\n", COLLECTOR_IP);
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    printf("[Sensor] Target collector: %s:%d\n", COLLECTOR_IP, COLLECTOR_PORT);

    for (int i = 1; i <= NUM_SAMPLES; i++) {
        double load1 = 0.0;
        long mem_total = 0, mem_free = 0;

        if (read_load1(&load1) == -1) {
            close(sock_fd);
            exit(EXIT_FAILURE);
        }
        if (read_meminfo(&mem_total, &mem_free) == -1) {
            close(sock_fd);
            exit(EXIT_FAILURE);
        }

        double temp = 40.0 + load1 * 10.0;
        double mem_used_pct = (double)(mem_total - mem_free) / (double)mem_total * 100.0;

        char message[BUFFER_SIZE];
        snprintf(message, sizeof(message),
                 "id=%s temp=%.1f mem_used=%.1f%%",
                 SENSOR_ID, temp, mem_used_pct);

        ssize_t sent = sendto(sock_fd, message, strlen(message), 0,
                               (struct sockaddr *)&collector_addr,
                               sizeof(collector_addr));
        if (sent == -1) {
            perror("sendto");
            close(sock_fd);
            exit(EXIT_FAILURE);
        }

        printf("[Sent %d/%d] %s\n", i, NUM_SAMPLES, message);
        fflush(stdout);

        if (i < NUM_SAMPLES) {
            sleep(2);
        }
    }

    printf("[Sensor] Done.\n");
    close(sock_fd);
    return 0;
}