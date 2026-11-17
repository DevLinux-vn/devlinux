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

static double get_temperature(void)
{
    FILE *fp;
    float load1 = 0.0f;

    fp = fopen("/proc/loadavg", "r");
    if (fp == NULL)
    {
        return 40.0;
    }

    fscanf(fp, "%f", &load1);
    fclose(fp);

    return 40.0 + load1 * 10.0;
}

static double get_mem_used_percent(void)
{
    FILE *fp;
    char line[256];

    long mem_total = 0;
    long mem_free = 0;

    fp = fopen("/proc/meminfo", "r");
    if (fp == NULL)
    {
        return 0.0;
    }

    while (fgets(line, sizeof(line), fp))
    {
        sscanf(line, "MemTotal: %ld kB", &mem_total);
        sscanf(line, "MemFree: %ld kB", &mem_free);

        if (mem_total > 0 && mem_free > 0)
        {
            break;
        }
    }

    fclose(fp);

    return ((double)(mem_total - mem_free) /
            mem_total) * 100.0;
}

int main(void)
{
    int sockfd;
    int i;

    struct sockaddr_in collector_addr;

    char message[BUFFER_SIZE];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0)
    {
        perror("socket");
        return 1;
    }

    memset(&collector_addr, 0, sizeof(collector_addr));

    collector_addr.sin_family = AF_INET;
    collector_addr.sin_port = htons(COLLECTOR_PORT);

    if (inet_pton(AF_INET,
                  COLLECTOR_IP,
                  &collector_addr.sin_addr) <= 0)
    {
        perror("inet_pton");
        close(sockfd);
        return 1;
    }

    printf("[Sensor] Target collector: %s:%d\n",
           COLLECTOR_IP,
           COLLECTOR_PORT);

    for (i = 1; i <= 5; i++)
    {
        double temp = get_temperature();
        double mem_used = get_mem_used_percent();

        snprintf(message,
                 sizeof(message),
                 "id=sensor-01 temp=%.1f mem_used=%.1f%%",
                 temp,
                 mem_used);

        if (sendto(sockfd,
                   message,
                   strlen(message),
                   0,
                   (struct sockaddr *)&collector_addr,
                   sizeof(collector_addr)) < 0)
        {
            perror("sendto");
            close(sockfd);
            return 1;
        }

        printf("[Sent %d/5] %s\n",
               i,
               message);

        sleep(2);
    }

    printf("[Sensor] Done.\n");

    close(sockfd);

    return 0;
}