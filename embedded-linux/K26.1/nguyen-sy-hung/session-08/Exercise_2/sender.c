#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define COLLECTOR_IP           "127.0.0.1"
#define COLLECTOR_PORT         9000U

#define SENSOR_ID              "sensor-01"

#define BUFFER_SIZE            256U
#define LOADAVG_SIZE           64U
#define MEMINFO_LINE_SIZE      128U

#define SEND_COUNT             5U
#define SEND_INTERVAL_SEC      2U

/*------------------------------------------------------------------*/

static int create_sender_socket(void)
{
    int socket_fd;

    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd == -1)
    {
        perror("socket");
        return -1;
    }

    return socket_fd;
}

/*------------------------------------------------------------------*/

static int read_cpu_temperature(double *temperature)
{
    FILE *fp;

    double load_avg;

    fp = fopen("/proc/loadavg", "r");
    if (fp == NULL)
    {
        perror("fopen");
        return -1;
    }

    if (fscanf(fp, "%lf", &load_avg) != 1)
    {
        fprintf(stderr, "Failed to read load average\n");
        fclose(fp);
        return -1;
    }

    fclose(fp);

    *temperature = 40.0 + (load_avg * 10.0);

    return 0;
}

/*------------------------------------------------------------------*/

static int read_memory_usage(double *mem_used_percent)
{
    FILE *fp;

    char line[MEMINFO_LINE_SIZE];

    long mem_total = 0;
    long mem_free = 0;

    fp = fopen("/proc/meminfo", "r");
    if (fp == NULL)
    {
        perror("fopen");
        return -1;
    }

    while (fgets(line, sizeof(line), fp) != NULL)
    {
        if (sscanf(line, "MemTotal: %ld", &mem_total) == 1)
        {
            continue;
        }

        if (sscanf(line, "MemFree: %ld", &mem_free) == 1)
        {
            continue;
        }

        if ((mem_total != 0) && (mem_free != 0))
        {
            break;
        }
    }

    fclose(fp);

    if (mem_total == 0)
    {
        fprintf(stderr, "Invalid MemTotal\n");
        return -1;
    }

    *mem_used_percent =
        ((double)(mem_total - mem_free) / (double)mem_total) * 100.0;

    return 0;
}

/*------------------------------------------------------------------*/

static void build_message(char *buffer,
                          size_t size,
                          double temperature,
                          double mem_used_percent)
{
    snprintf(buffer,
             size,
             "id=%s temp=%.1f mem_used=%.1f%%",
             SENSOR_ID,
             temperature,
             mem_used_percent);
}

/*------------------------------------------------------------------*/

static int send_telemetry(int socket_fd,
                          const struct sockaddr_in *collector_addr,
                          const char *message)
{
    ssize_t sent_bytes;

    sent_bytes = sendto(socket_fd,
                        message,
                        strlen(message),
                        0,
                        (const struct sockaddr *)collector_addr,
                        sizeof(*collector_addr));

    if (sent_bytes == -1)
    {
        perror("sendto");
        return -1;
    }

    return 0;
}

/*------------------------------------------------------------------*/

int main(void)
{
    int socket_fd;

    struct sockaddr_in collector_addr;

    char message[BUFFER_SIZE];

    double temperature;
    double mem_used_percent;

    unsigned int count;

    socket_fd = create_sender_socket();
    if (socket_fd == -1)
    {
        return EXIT_FAILURE;
    }

    memset(&collector_addr, 0, sizeof(collector_addr));

    collector_addr.sin_family = AF_INET;
    collector_addr.sin_port = htons(COLLECTOR_PORT);

    if (inet_pton(AF_INET,
                  COLLECTOR_IP,
                  &collector_addr.sin_addr) != 1)
    {
        perror("inet_pton");
        close(socket_fd);
        return EXIT_FAILURE;
    }

    printf("[Sensor] Target collector: %s:%u\n",
           COLLECTOR_IP,
           COLLECTOR_PORT);

    for (count = 1; count <= SEND_COUNT; count++)
    {
        if (read_cpu_temperature(&temperature) == -1)
        {
            break;
        }

        if (read_memory_usage(&mem_used_percent) == -1)
        {
            break;
        }

        build_message(message,
                      sizeof(message),
                      temperature,
                      mem_used_percent);

        if (send_telemetry(socket_fd,
                           &collector_addr,
                           message) == -1)
        {
            break;
        }

        printf("[Sent %u/%u] %s\n",
               count,
               SEND_COUNT,
               message);

        if (count < SEND_COUNT)
        {
            sleep(SEND_INTERVAL_SEC);
        }
    }

    if (close(socket_fd) == -1)
    {
        perror("close");
        return EXIT_FAILURE;
    }

    printf("[Sensor] Done.\n");

    return EXIT_SUCCESS;
}