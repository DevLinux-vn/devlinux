#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#define COLLECTOR_IP "127.0.0.1"
#define COLLECTOR_PORT 9000
#define BUFFER_SIZE 256
#define TEMP_BASE 40.0
#define TEMP_LOAD_FACTOR 10.0

static int read_temp_simulated(double *temp) {
    FILE *f = fopen("/proc/loadavg", "r");
    if (!f) {
        perror("fopen /proc/loadavg");
        return -1;
    }
    float load1;
    if (fscanf(f, "%f", &load1) != 1) {
        fclose(f);
        return -1;
    }
    fclose(f);
    *temp = TEMP_BASE + load1 * TEMP_LOAD_FACTOR;
    return 0;
}

static int read_mem_used_percent(double *mem_used_pct) {
    FILE *f = fopen("/proc/meminfo", "r");
    if (!f) {
        perror("fopen /proc/meminfo");
        return -1;
    }
    char line[128];
    long mem_total = -1, mem_free = -1;
    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, "MemTotal:", 9) == 0) {
            sscanf(line, "MemTotal: %ld kB", &mem_total);
        } else if (strncmp(line, "MemFree:", 8) == 0) {
            sscanf(line, "MemFree: %ld kB", &mem_free);
        }
    }
    fclose(f);
    
    if (mem_total == -1 || mem_free == -1 || mem_total == 0) {
        return -1;
    }
    
    *mem_used_pct = (double)(mem_total - mem_free) / (double)mem_total * 100.0;
    return 0;
}

int main(void) {
    int client_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_fd == -1) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in collector_addr;
    memset(&collector_addr, 0, sizeof(collector_addr));
    collector_addr.sin_family = AF_INET;
    collector_addr.sin_port = htons(COLLECTOR_PORT);
    if (inet_pton(AF_INET, COLLECTOR_IP, &collector_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(client_fd);
        return 1;
    }

    printf("[Sensor] Target collector: %s:%d\n", COLLECTOR_IP, COLLECTOR_PORT);

    for (int i = 1; i <= 5; ++i) {
        double temp = 0.0;
        if (read_temp_simulated(&temp) == -1) {
            fprintf(stderr, "Failed to read temperature\n");
            continue;
        }

        double mem_used_pct = 0.0;
        if (read_mem_used_percent(&mem_used_pct) == -1) {
            fprintf(stderr, "Failed to read memory usage\n");
            continue;
        }

        char message[BUFFER_SIZE];
        int n = snprintf(message, sizeof(message), "id=sensor-01 temp=%.1f mem_used=%.1f%%", temp, mem_used_pct);
        if (n < 0 || n >= (int)sizeof(message)) {
            fprintf(stderr, "snprintf truncated\n");
            continue;
        }

        ssize_t ret;
        do {
            ret = sendto(client_fd, message, strlen(message), 0,
                         (struct sockaddr *)&collector_addr, sizeof(collector_addr));
        } while (ret == -1 && errno == EINTR);

        if (ret == -1) {
            perror("sendto");
            continue;
        }

        printf("[Sent %d/5] %s\n", i, message);

        if (i < 5) {
            sleep(2);
        }
    }

    printf("[Sensor] Done.\n");
    close(client_fd);
    return 0;
}
