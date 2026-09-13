#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define COLLECTOR_IP "127.0.0.1"
#define COLLECTOR_PORT 9000
#define BUFFER_SIZE 128

static int read_cpu_load(double *load)
{
    FILE *fp;
    char buffer[256];

    if (load == NULL)
    {
        return -1;
    }

    fp = fopen("/proc/loadavg", "r");
    if (fp == NULL)
    {
        perror("fopen /proc/loadavg");
        return -1;
    }

    if (fgets(buffer, sizeof(buffer), fp) == NULL)
    {
        perror("fgets /proc/loadavg");
        fclose(fp);
        return -1;
    }

    fclose(fp);

    if (sscanf(buffer, "%lf", load) != 1)
    {
        fprintf(stderr, "Failed to parse CPU load\n");
        return -1;
    }

    return 0;
}

static int read_memory(unsigned long *mem_total,
                       unsigned long *mem_free)
{
    FILE *fp;
    char buffer[256];

    if (mem_total == NULL || mem_free == NULL)
    {
        return -1;
    }

    *mem_total = 0;
    *mem_free = 0;

    fp = fopen("/proc/meminfo", "r");
    if (fp == NULL)
    {
        perror("fopen /proc/meminfo");
        return -1;
    }

    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        if (sscanf(buffer, "MemTotal: %lu kB", mem_total) == 1)
        {
            continue;
        }

        if (sscanf(buffer, "MemFree: %lu kB", mem_free) == 1)
        {
            continue;
        }
    }

    fclose(fp);

    if (*mem_total == 0)
    {
        fprintf(stderr, "MemTotal not found\n");
        return -1;
    }

    return 0;
}

int main(void)
{
    int sockfd;
    struct sockaddr_in collector_addr;

    char message[BUFFER_SIZE];

    double load1;
    double temperature;

    unsigned long mem_total;
    unsigned long mem_free;
    double mem_used_pct;

    printf("[Sensor] Target collector: %s:%d\n",
           COLLECTOR_IP,
           COLLECTOR_PORT);

    /*
     * Create UDP socket
     *
     * AF_INET    -> IPv4
     * SOCK_DGRAM -> UDP
     */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("socket");
        return EXIT_FAILURE;
    }

    /*
     * Configure collector address
     */
    memset(&collector_addr, 0, sizeof(collector_addr));

    collector_addr.sin_family = AF_INET;
    collector_addr.sin_port = htons(COLLECTOR_PORT);

    if (inet_pton(AF_INET,
                  COLLECTOR_IP,
                  &collector_addr.sin_addr) <= 0)
    {
        perror("inet_pton");
        close(sockfd);
        return EXIT_FAILURE;
    }

    /*
     * Send exactly 5 datagrams
     */
    for (int i = 0; i < 5; i++)
    {
        /*
         * Read CPU load
         */
        if (read_cpu_load(&load1) != 0)
        {
            close(sockfd);
            return EXIT_FAILURE;
        }

        /*
         * Simulated temperature
         *
         * temp = 40.0 + load1 * 10
         */
        temperature = 40.0 + (load1 * 10.0);

        /*
         * Read memory information
         */
        if (read_memory(&mem_total, &mem_free) != 0)
        {
            close(sockfd);
            return EXIT_FAILURE;
        }

        /*
         * Calculate RAM usage
         */
        mem_used_pct =
            ((double)(mem_total - mem_free) /
             (double)mem_total) * 100.0;

        /*
         * Build telemetry message
         */
        int len = snprintf(
            message,
            sizeof(message),
            "id=sensor-01 temp=%.1f mem_used=%.1f%%",
            temperature,
            mem_used_pct
        );

        if (len < 0 || (size_t)len >= sizeof(message))
        {
            fprintf(stderr, "Message is too long\n");
            close(sockfd);
            return EXIT_FAILURE;
        }

        /*
         * Send UDP datagram
         */
        ssize_t sent_bytes = sendto(
            sockfd,
            message,
            (size_t)len,
            0,
            (struct sockaddr *)&collector_addr,
            sizeof(collector_addr)
        );

        if (sent_bytes < 0)
        {
            perror("sendto");
            close(sockfd);
            return EXIT_FAILURE;
        }

        if (sent_bytes != len)
        {
            fprintf(stderr, "Incomplete UDP datagram sent\n");
            close(sockfd);
            return EXIT_FAILURE;
        }

        printf("[Sent %d/5] %s\n", i + 1, message);

        /*
         * Wait 2 seconds between transmissions.
         * No need to sleep after the final packet.
         */
        if (i < 4)
        {
            sleep(2);
        }
    }

    printf("[Sensor] Done.\n");

    if (close(sockfd) < 0)
    {
        perror("close");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
