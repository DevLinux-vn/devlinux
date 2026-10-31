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

void get_system_metrics(double *temp, double *mem_used_pct) {
    // Read CPU Load from /proc/loadavg
    FILE *f_load = fopen("/proc/loadavg", "r");
    double load1 = 0.0;
    if (f_load) {
        if (fscanf(f_load, "%lf", &load1) != 1) {
            load1 = 0.0;
        }
        fclose(f_load);
    }
    *temp = 40.0 + load1 * 10.0;

    // Read RAM from /proc/meminfo
    FILE *f_mem = fopen("/proc/meminfo", "r");
    long long mem_total = 0, mem_free = 0;
    if (f_mem) {
        char line[128];
        while (fgets(line, sizeof(line), f_mem)) {
            if (sscanf(line, "MemTotal: %lld kB", &mem_total) == 1) continue;
            if (sscanf(line, "MemFree: %lld kB", &mem_free) == 1) continue;
        }
        fclose(f_mem);
    }
    
    if (mem_total > 0) {
        *mem_used_pct = (double)(mem_total - mem_free) / mem_total * 100.0;
    } else {
        *mem_used_pct = 0.0;
    }
}

int main() {
    // 1. socket(AF_INET, SOCK_DGRAM, 0)
    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 2. Configure collector address
    struct sockaddr_in collector_addr;
    memset(&collector_addr, 0, sizeof(collector_addr));
    collector_addr.sin_family = AF_INET;
    collector_addr.sin_port = htons(COLLECTOR_PORT);
    if (inet_pton(AF_INET, COLLECTOR_IP, &collector_addr.sin_addr) <= 0) {
        perror("Invalid address / Address not supported");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    printf("[Sensor] Target collector: %s:%d\n", COLLECTOR_IP, COLLECTOR_PORT);

    char msg_buf[BUFFER_SIZE];
    double temp = 0.0, mem_used_pct = 0.0;

    // 3. Loop exactly 5 times then exit
    for (int i = 1; i <= 5; i++) {
        // Collect metrics
        get_system_metrics(&temp, &mem_used_pct);

        // Format datagram payload string
        snprintf(msg_buf, sizeof(msg_buf), "id=sensor-01 temp=%.1f mem_used=%.1f%%", temp, mem_used_pct);

        // Send datagram using sendto()
        ssize_t sent_bytes = sendto(sock_fd, msg_buf, strlen(msg_buf), 0,
                                    (struct sockaddr *)&collector_addr, sizeof(collector_addr));
        if (sent_bytes < 0) {
            perror("sendto failed");
            break;
        }

        // Print to log
        printf("[Sent %d/5] %s\n", i, msg_buf);

        // Wait for 2 seconds before next message
        if (i < 5) {
            sleep(2);
        }
    }

    printf("[Sensor] Done.\n");
    close(sock_fd);
    return 0;
}