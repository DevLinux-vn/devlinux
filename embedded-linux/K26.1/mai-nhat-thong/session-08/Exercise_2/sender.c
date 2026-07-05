/**
 * @file sender.c
 * @brief Simulated IoT Sensor Device sending logs via UDP.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define COLLECTOR_IP "127.0.0.1"
#define COLLECTOR_PORT 9000
#define MAX_ITERATIONS 5
#define BUFFER_SIZE 256

/**
 * @brief Reads /proc metrics and calculates pseudo-sensor values.
 */
void get_sensor_data(double *temp, double *mem_used_pct) {
    /* 1. Calculate Temperature from Load Average */
    FILE *f_load = fopen("/proc/loadavg", "r");
    if (f_load) {
        float load1;
        if (fscanf(f_load, "%f", &load1) == 1) {
            *temp = 40.0 + (load1 * 10.0);
        }
        fclose(f_load);
    }

    /* 2. Calculate RAM Usage Percentage */
    FILE *f_mem = fopen("/proc/meminfo", "r");
    if (f_mem) {
        char line[128];
        long mem_total = 0, mem_free = 0;
        
        while (fgets(line, sizeof(line), f_mem)) {
            if (strncmp(line, "MemTotal:", 9) == 0) {
                sscanf(line, "MemTotal: %ld kB", &mem_total);
            } else if (strncmp(line, "MemFree:", 8) == 0) {
                sscanf(line, "MemFree: %ld kB", &mem_free);
            }
        }
        fclose(f_mem);

        if (mem_total > 0) {
            *mem_used_pct = ((double)(mem_total - mem_free) / mem_total) * 100.0;
        }
    }
}

int main(void) {
    int sock_fd;
    struct sockaddr_in collector_addr;
    char buffer[BUFFER_SIZE];

    /* 1. Create UDP Socket */
    if ((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    /* 2. Configure Collector Destination Address */
    memset(&collector_addr, 0, sizeof(collector_addr));
    collector_addr.sin_family = AF_INET;
    collector_addr.sin_port = htons(COLLECTOR_PORT);
    if (inet_pton(AF_INET, COLLECTOR_IP, &collector_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    printf("[Sensor] Target collector: %s:%d\n", COLLECTOR_IP, COLLECTOR_PORT);

    /* 3. Execution Loop */
    for (int i = 1; i <= MAX_ITERATIONS; i++) {
        double temp = 40.0; /* Default fail-safe values */
        double mem_used_pct = 0.0;

        get_sensor_data(&temp, &mem_used_pct);

        /* Construct Datagram Payload */
        snprintf(buffer, sizeof(buffer), "id=sensor-01 temp=%.1f mem_used=%.1f%%", 
                 temp, mem_used_pct);

        /* Send Datagram */
        ssize_t sent_bytes = sendto(sock_fd, buffer, strlen(buffer), 0, 
                                   (struct sockaddr *)&collector_addr, 
                                   sizeof(collector_addr));
        if (sent_bytes == -1) {
            perror("sendto failed");
        } else {
            printf("[Sent %d/%d] %s\n", i, MAX_ITERATIONS, buffer);
        }

        sleep(2);
    }

    printf("[Sensor] Done.\n");
    close(sock_fd);
    return EXIT_SUCCESS;
}