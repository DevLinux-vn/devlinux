#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define COLLECTOR_IP "127.0.0.1"
#define COLLECTOR_PORT 9000
#define BUFFER_SIZE 256
#define TOTAL_BURSTS 5
#define SLEEP_INTERVAL_SEC 2

static void collect_telemetry_metrics(double *temp_out, double *mem_used_out) {
    double load1 = 0.0;
    FILE *f_cpu = fopen("/proc/loadavg", "r");
    if (f_cpu != NULL) {
        if (fscanf(f_cpu, "%lf", &load1) != 1) load1 = 0.0;
        fclose(f_cpu);
    }
    *temp_out = 40.0 + (load1 * 10.0);

    long mem_total = 1, mem_free = 0; /* Gán bằng 1 để tránh lỗi chia cho 0 trong lập trình phòng thủ */
    char line[128];
    FILE *f_mem = fopen("/proc/meminfo", "r");
    if (f_mem != NULL) {
        while (fgets(line, sizeof(line), f_mem)) {
            if (strncmp(line, "MemTotal:", 9) == 0) {
                sscanf(line, "MemTotal: %ld", &mem_total);
            } else if (strncmp(line, "MemFree:", 8) == 0) {
                sscanf(line, "MemFree: %ld", &mem_free);
            }
        }
        fclose(f_mem);
    }
    *mem_used_out = (double)(mem_total - mem_free) / (double)mem_total * 100.0;
}

int main(void) {
    setbuf(stdout, NULL);

    int client_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_fd < 0) {
        perror("CRITICAL: Socket creation failed");
        return EXIT_FAILURE;
    }

    struct sockaddr_in target_addr;
    memset(&target_addr, 0, sizeof(target_addr));
    target_addr.sin_family = AF_INET;
    target_addr.sin_port = htons(COLLECTOR_PORT);
    if (inet_pton(AF_INET, COLLECTOR_IP, &target_addr.sin_addr) <= 0) {
        perror("CRITICAL: Invalid address configuration");
        close(client_fd);
        return EXIT_FAILURE;
    }

    printf("[Sensor] Target collector: %s:%d\n", COLLECTOR_IP, COLLECTOR_PORT);

    for (int i = 1; i <= TOTAL_BURSTS; i++) {
        double temp = 0.0, mem_used = 0.0;
        collect_telemetry_metrics(&temp, &mem_used);

        char payload[BUFFER_SIZE];
        snprintf(payload, sizeof(payload), "id=sensor-01 temp=%.1f mem_used=%.1f%%", temp, mem_used);

        ssize_t sent_bytes = sendto(client_fd, payload, strlen(payload), 0,
                                    (struct sockaddr *)&target_addr, sizeof(target_addr));
        if (sent_bytes < 0) {
            perror("ERROR: Telemetry delivery failed");
        } else {
            printf("[Sent %d/%d] %s\n", i, TOTAL_BURSTS, payload);
        }

        sleep(SLEEP_INTERVAL_SEC);
    }

    printf("[Sensor] Done.\n");
    close(client_fd);
    return EXIT_SUCCESS;
}