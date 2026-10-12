#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#define COLLECTOR_IP   "127.0.0.1"
#define COLLECTOR_PORT 9000
#define BUFFER_SIZE    128

int receiver_fd;

void sigint_handler(int sig)
{
    close(receiver_fd);
    printf("[Collector] Server closed.\n");
    exit(0);
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
    
    receiver_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (receiver_fd <= 0)
    {
        printf("[ERROR] socket initial\n");
        return 1;
    }
    
    /* Configure Unix Domain Socket address */
    struct sockaddr_in addr;
    struct sockaddr_in client_addr;
    socklen_t sender_len = sizeof(client_addr);
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(COLLECTOR_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    return_value = bind(receiver_fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
    if(return_value != 0)
    {
        printf("[ERROR] bind error\n");
        return 1;
    }
    
    printf("[Collector] Listening on %d:%d...\n", INADDR_ANY, COLLECTOR_PORT);
    while(1)
    {
        return_value = recvfrom(receiver_fd, buffer, sizeof(buffer), 0, (struct sockaddr *) &client_addr, &sender_len);
 
         if (return_value == -1)
        {
            printf("[ERROR] recv error\n");
            return 1;
        }   
        char client_ip[INET_ADDRSTRLEN];
        if (inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip)) == NULL)
        {
            perror("inet_ntop");
        }
        /* Generate timestamp */
        time_t now = time(NULL);
        struct tm *t = localtime(&now);
        char ts[16];
        strftime(ts, sizeof(ts), "%H:%M:%S", t);
        printf("[%s] %s:%u -> %s\n", ts, client_ip, (unsigned int)ntohs(client_addr.sin_port), buffer);
    }

    close(receiver_fd);
    printf("[Collector] Server closed.\n");
    return 0;
}