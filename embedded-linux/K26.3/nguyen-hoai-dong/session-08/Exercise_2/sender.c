#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define COLLECTOR_IP   "127.0.0.1"
#define COLLECTOR_PORT 9000
#define BUFFER_SIZE    128

int sender_fd;

float get_cpu_temperature(void)
{
    float load;
    FILE *f = fopen("/proc/loadavg", "r");
    if (f == NULL)
    {
        printf("[ERROR] open file fail\n");
        return 1;
    }
    fscanf(f, "%f", &load);
    fclose(f);

    return 40.0 + load*10;
}

float get_mem_used_percent(void)
{
    char line[128];
    unsigned long mem_total = 0;
    unsigned long mem_free = 0;

    FILE *f = fopen("/proc/meminfo", "r");
    if (f == NULL)
    {
        printf("[ERROR] open file fail\n");
        return 1;
    }

    while (fgets(line, sizeof(line), f) != NULL)
    {
        if (strncmp(line, "MemTotal:", 9) == 0)
        {
            sscanf(line, "MemTotal: %lu", &mem_total);
        }
        else if (strncmp(line, "MemFree:", 8) == 0)
        {
            sscanf(line, "MemFree: %lu", &mem_free);
        }
    }

    fclose(f);
    return (float)(mem_total - mem_free)/mem_total*100;
}

int main(void)
{
    int return_value = 0;
    char buffer[BUFFER_SIZE];
    char buffer_recv[BUFFER_SIZE];
    memset(&buffer, 0, sizeof(buffer));

    sender_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sender_fd <= 0)
    {
        printf("[ERROR] socket initial\n");
        return 1;
    }
    
    /* Configure Unix Domain Socket address */
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(COLLECTOR_PORT);
    if (inet_pton(AF_INET, COLLECTOR_IP, &addr.sin_addr) != 1)
    {
        printf("[ERROR] inet_pton error");
        return 1;
    }
    printf("[Sensor] Target collector: %s:%d\n", COLLECTOR_IP, COLLECTOR_PORT);
    for (int i = 0; i < 5; i++)
    {
        float cpu_temp = get_cpu_temperature();
        float mem_used_percent = get_mem_used_percent();
        
        snprintf(buffer, sizeof(buffer), "id=sensor-01 temp=%0.2f mem_used=%0.2f%%",cpu_temp, mem_used_percent);
        return_value = sendto(sender_fd, buffer, sizeof(buffer), 0, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
        if(return_value <= 0)
        {
            printf("[ERROR] sendto error\n");
        }
        printf("[Sent %d/5] id=sensor-01 temp=%0.2f mem_used=%0.2f%%\n", i+1, cpu_temp, mem_used_percent);
        sleep(2);
    }

    printf("[Sensor] Done.\n");
    close(sender_fd);
    return 0;
}