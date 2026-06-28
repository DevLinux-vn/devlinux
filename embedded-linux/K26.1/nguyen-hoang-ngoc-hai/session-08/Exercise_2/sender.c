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

double read_loadavg(void) {
    FILE *f = fopen("/proc/loadavg", "r");
    if (f == NULL) {
        perror("fopen /proc/loadavg");
        return -1.0;
    }

    double load1 = 0.0;

    if (fscanf(f, "%lf", &load1) != 1) {
        fprintf(stderr, "ERROR: cannot parse /proc/loadavg\n");
        fclose(f);
        return -1.0;
    }

    fclose(f);
    return load1;
}

int read_meminfo(long *mem_total, long *mem_free) {
    FILE *f = fopen("/proc/meminfo", "r");
    if (f == NULL) {
        perror("fopen /proc/meminfo");
        return -1;
    }

    char line[128];
    *mem_total = 0;
    *mem_free = 0;

    while (fgets(line, sizeof(line), f) != NULL) {
        if (sscanf(line, "MemTotal: %ld kB", mem_total) == 1) {
            continue;
        }

        if (sscanf(line, "MemFree: %ld kB", mem_free) == 1) {
            continue;
        }

        if (*mem_total > 0 && *mem_free > 0) {
            break;
        }
    }

    fclose(f);

    if (*mem_total <= 0 || *mem_free < 0) {
        fprintf(stderr, "ERROR: cannot parse /proc/meminfo\n");
        return -1;
    }

    return 0;
}

int main(void) {
    int sock_fd;
    struct sockaddr_in collector_addr;

    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&collector_addr, 0, sizeof(collector_addr));
    collector_addr.sin_family = AF_INET;
    collector_addr.sin_port = htons(COLLECTOR_PORT);

    if (inet_pton(AF_INET, COLLECTOR_IP, &collector_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    printf("[Sensor] Target collector: %s:%d\n", COLLECTOR_IP, COLLECTOR_PORT);

    for (int i = 1; i <= 5; i++) {
        char message[BUFFER_SIZE];

        double load1 = read_loadavg();
        if (load1 < 0) {
            close(sock_fd);
            exit(EXIT_FAILURE);
        }

        long mem_total;
        long mem_free;

        if (read_meminfo(&mem_total, &mem_free) == -1) {
            close(sock_fd);
            exit(EXIT_FAILURE);
        }

        double temp = 40.0 + load1 * 10.0;
        double mem_used_percent =
            (double)(mem_total - mem_free) / (double)mem_total * 100.0;

        snprintf(
            message,
            sizeof(message),
            "id=sensor-01 temp=%.1f mem_used=%.1f%%",
            temp,
            mem_used_percent
        );

        ssize_t sent_bytes = sendto(
            sock_fd,
            message,
            strlen(message),
            0,
            (struct sockaddr *)&collector_addr,
            sizeof(collector_addr)
        );

        if (sent_bytes == -1) {
            perror("sendto");
            close(sock_fd);
            exit(EXIT_FAILURE);
        }

        printf("[Sent %d/5] %s\n", i, message);

        sleep(2);
    }

    printf("[Sensor] Done.\n");

    close(sock_fd);
    return 0;
}