#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/monitor.sock"
#define BUFFER_SIZE 256

int server_fd, client_fd;

void sigint_handler(int sig)
{
    close(client_fd);
    close(server_fd);
    unlink(SOCKET_PATH);
    printf("[Daemon] Server closed.\n");
    exit(0);
}

float get_cpu(void)
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

    return load;
}

unsigned long get_memtotal(void)
{
    char line[128];
    unsigned long mem_total = 0;

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
            break;
        }
    }

    fclose(f);
    return mem_total;
}

unsigned long get_memfree(void)
{
    char line[128];
    unsigned long mem_free = 0;

    FILE *f = fopen("/proc/meminfo", "r");
    if (f == NULL)
    {
        printf("[ERROR] open file fail\n");
        return 1;
    }

    while (fgets(line, sizeof(line), f) != NULL)
    {
        if (strncmp(line, "MemFree:", 8) == 0)
        {
            sscanf(line, "MemFree: %lu", &mem_free);
            break;
        }
    }

    fclose(f);
    return mem_free;
}

int main(void)
{
    int return_value = 0;
    char buffer[BUFFER_SIZE];
    memset(&buffer, 0, sizeof(buffer));
    if (signal(SIGINT, sigint_handler) != 0)
    {
        printf("[ERROR] signal initial\n");
        return 1;
    }
    
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd <= 0)
    {
        printf("[ERROR] socket initial\n");
        return 1;
    }

    /* Configure Unix Domain Socket address */
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);
    return_value = bind(server_fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un));
    if(return_value != 0)
    {
        printf("[ERROR] bind error\n");
        return 1;
    }
    printf("[Daemon] Listening on %s\n", SOCKET_PATH);
    return_value = listen(server_fd, 1);
    if(return_value != 0)
    {
        printf("[ERROR] listen error\n");
        return 1;
    }

    while(1)
    {
        client_fd = accept(server_fd, NULL, NULL);
        if (client_fd <= 0)
        {
            printf("[ERROR] accept error\n");
            return 1;
        }
        printf("[Daemon] Client connected.\n");
        while(1)
        {
            return_value = recv(client_fd, buffer, BUFFER_SIZE, 0);
            if(return_value == 0)
            {
                printf("[Daemon] Client disconnected\n");
                break;
            } 
            else if (return_value == -1)
            {
                printf("[ERROR] recv error\n");
                return 1;
            }   

            if(strcmp(buffer, "cpu") == 0)
            {
                float cpu_value = get_cpu();
                return_value = send(client_fd, &cpu_value, sizeof(cpu_value), 0);
                if (return_value == -1)
                {
                    printf("[ERROR] send error\n");
                    return 1;
                }   
                printf("[Daemon] [CMD] cpu\n");
            }
            else if (strcmp(buffer, "mem") == 0)
            {
                unsigned long mem_total = get_memtotal();
                unsigned long mem_free = get_memfree();
                return_value = send(client_fd, &mem_total, sizeof(mem_total), 0);
                if (return_value == -1)
                {
                    printf("[ERROR] send error\n");
                    return 1;
                }
                return_value = send(client_fd, &mem_free, sizeof(mem_total), 0);
                if (return_value == -1)
                {
                    printf("[ERROR] send error\n");
                    return 1;
                }
                printf("[Daemon] [CMD] mem\n");
            }
            else if (strcmp(buffer, "quit") == 0)
            {
                break;
            }
            else
            {               
                memcpy(buffer, "ERROR: unknown command", 23);
                return_value = send(client_fd, &buffer, sizeof(buffer), 0);
                if (return_value == -1)
                {
                    printf("[ERROR] send error\n");
                    return 1;
                }
                printf("[Daemon] [ERROR] unknown CMD\n");
            }
        }
        close(client_fd);
        printf("[Daemon] Client disconnected. Waiting for next client...\n");
        
    }

    close(client_fd);
    close(server_fd);
    unlink(SOCKET_PATH);
    printf("[Daemon] Server closed.\n");
    return 0;
}