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
#define TOTAL_MESSAGES 5

double read_cpu_temperature(void)
{
    FILE *file;
    char line[128];

    double load1;

    file = fopen("/proc/loadavg", "r");

    if (file == NULL) {
        perror("fopen /proc/loadavg");
        return -1.0;
    }

    if (fgets(line, sizeof(line), file) == NULL) {
        perror("fgets /proc/loadavg");
        fclose(file);
        return -1.0;
    }

    fclose(file);

    if (sscanf(line, "%lf", &load1) != 1) {
        fprintf(stderr, "Failed to parse load average\n");
        return -1.0;
    }

    /*
     * Simulated CPU temperature:
     * temperature = 40.0 + load1 * 10
     */
    return 40.0 + load1 * 10.0;
}

double read_memory_usage(void)
{
    FILE *file;
    char line[128];

    long mem_total = 0;
    long mem_free = 0;

    file = fopen("/proc/meminfo", "r");

    if (file == NULL) {
        perror("fopen /proc/meminfo");
        return -1.0;
    }

    while (fgets(line, sizeof(line), file) != NULL) {

        if (strncmp(line, "MemTotal:", 9) == 0) {

            if (sscanf(line,
                       "MemTotal: %ld kB",
                       &mem_total) != 1) {

                fprintf(stderr,
                        "Failed to parse MemTotal\n");

                fclose(file);
                return -1.0;
            }
        }

        if (strncmp(line, "MemFree:", 8) == 0) {

            if (sscanf(line,
                       "MemFree: %ld kB",
                       &mem_free) != 1) {

                fprintf(stderr,
                        "Failed to parse MemFree\n");

                fclose(file);
                return -1.0;
            }
        }

        if (mem_total > 0 && mem_free > 0) {
            break;
        }
    }

    fclose(file);

    if (mem_total <= 0) {
        fprintf(stderr, "Invalid MemTotal\n");
        return -1.0;
    }

    double mem_used_pct =
        ((double)(mem_total - mem_free)
        / mem_total) * 100.0;

    return mem_used_pct;
}

int main(void)
{
    int sockfd;

    struct sockaddr_in collector_addr;

    char message[BUFFER_SIZE];

    /* Create UDP socket */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd == -1) {
        perror("socket");
        return 1;
    }

    /* Configure collector address */
    memset(&collector_addr, 0, sizeof(collector_addr));

    collector_addr.sin_family = AF_INET;
    collector_addr.sin_port = htons(COLLECTOR_PORT);

    if (inet_pton(AF_INET,
                  COLLECTOR_IP,
                  &collector_addr.sin_addr) != 1) {

        fprintf(stderr,
                "Invalid collector IP address: %s\n",
                COLLECTOR_IP);

        close(sockfd);
        return 1;
    }

    printf("[Sensor] Target collector: %s:%d\n",
           COLLECTOR_IP,
           COLLECTOR_PORT);

    for (int i = 1; i <= TOTAL_MESSAGES; i++) {

        double temperature = read_cpu_temperature();

        if (temperature < 0) {
            close(sockfd);
            return 1;
        }

        double mem_used_pct = read_memory_usage();

        if (mem_used_pct < 0) {
            close(sockfd);
            return 1;
        }

        /* Build telemetry message */
        snprintf(message,
                 sizeof(message),
                 "id=sensor-01 temp=%.1f mem_used=%.1f%%",
                 temperature,
                 mem_used_pct);

        /* Send UDP datagram */
        ssize_t bytes_sent = sendto(
            sockfd,
            message,
            strlen(message),
            0,
            (struct sockaddr *)&collector_addr,
            sizeof(collector_addr)
        );

        if (bytes_sent == -1) {
            perror("sendto");
            close(sockfd);
            return 1;
        }

        printf("[Sent %d/%d] %s\n",
               i,
               TOTAL_MESSAGES,
               message);

        if (i < TOTAL_MESSAGES) {
            sleep(2);
        }
    }

    printf("[Sensor] Done.\n");

    if (close(sockfd) == -1) {
        perror("close");
        return 1;
    }

    return 0;
}
