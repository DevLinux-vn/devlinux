#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* --- Constants --- */
#define COLLECTOR_IP        "127.0.0.1"
#define COLLECTOR_PORT      9000

#define MAX_ITERATIONS      5
#define SLEEP_INTERVAL_SEC  2

#define MAX_BUFFER_SIZE     256
#define MAX_LINE_SIZE       128

#define FILE_LOADAVG        "/proc/loadavg"
#define FILE_MEMINFO        "/proc/meminfo"
#define SENSOR_ID           "sensor-01"

/* --- Helper: Simulate Temp from LoadAvg --- */
static int get_simulated_temp(double *temp_out) {
    FILE *file = fopen(FILE_LOADAVG, "r");
    if (!file) return -1;

    double load1;
    if (fscanf(file, "%lf", &load1) == 1) {
        *temp_out = 40.0 + (load1 * 10.0);
        fclose(file);
        return 0;
    }
    fclose(file);
    return -1;
}

/* --- Helper: Get Memory Usage Percent --- */
static int get_mem_used_pct(double *pct_out) {
    FILE *file = fopen(FILE_MEMINFO, "r");
    if (!file) return -1;

    long mem_total = -1;
    long mem_free = -1;
    char line[MAX_LINE_SIZE];

    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "MemTotal:", 9) == 0) {
            sscanf(line + 9, "%ld", &mem_total);
        } else if (strncmp(line, "MemFree:", 8) == 0) {
            sscanf(line + 8, "%ld", &mem_free);
        }
    }
    fclose(file);

    if (mem_total > 0 && mem_free >= 0) {
        *pct_out = (double)(mem_total - mem_free) / mem_total * 100.0;
        return 0;
    }
    return -1;
}

int main(void) {
    /* 1. Create UDP Socket */
    int sensor_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sensor_fd < 0) {
        perror("[Sensor] socket failed");
        exit(EXIT_FAILURE);
    }

    /* 2. Configure Target Address */
    struct sockaddr_in target_addr;
    memset(&target_addr, 0, sizeof(target_addr));
    target_addr.sin_family = AF_INET;
    target_addr.sin_port   = htons(COLLECTOR_PORT);
    
    if (inet_pton(AF_INET, COLLECTOR_IP, &target_addr.sin_addr) <= 0) {
        perror("[Sensor] invalid address / address not supported");
        close(sensor_fd);
        exit(EXIT_FAILURE);
    }

    printf("[Sensor] Target collector: %s:%d\n", COLLECTOR_IP, COLLECTOR_PORT);

    /* 3. Send Loop */
    for (int i = 1; i <= MAX_ITERATIONS; ++i) {
        double temp = 0.0;
        double mem_pct = 0.0;
        char message[MAX_BUFFER_SIZE];

        /* Gather sensor data */
        if (get_simulated_temp(&temp) < 0) temp = 0.0;
        if (get_mem_used_pct(&mem_pct) < 0) mem_pct = 0.0;

        /* Format datagram */
        snprintf(message, sizeof(message), "id=%s temp=%.1f mem_used=%.1f%%", SENSOR_ID, temp, mem_pct);

        /* Send UDP datagram */
        ssize_t bytes_sent = sendto(sensor_fd, message, strlen(message), 0,
                                    (struct sockaddr *)&target_addr, sizeof(target_addr));
        if (bytes_sent < 0) {
            perror("[Sensor] sendto failed");
        } else {
            printf("[Sent %d/%d] %s\n", i, MAX_ITERATIONS, message);
        }

        /* Sleep before next transmission (unless it's the last iteration) */
        if (i < MAX_ITERATIONS) {
            sleep(SLEEP_INTERVAL_SEC);
        }
    }

    printf("[Sensor] Done.\n");
    close(sensor_fd);
    
    return EXIT_SUCCESS;
}