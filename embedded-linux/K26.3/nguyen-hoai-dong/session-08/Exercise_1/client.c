#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/monitor.sock"
#define BUFFER_SIZE 256

int client_fd;

void sigint_handler(int sig)
{
    close(client_fd);
    printf("[monitor-cli] Closed\n");
    exit(0);
}

int main(void)
{
    int return_value = 0;
    char buffer[BUFFER_SIZE];
    char buffer_recv[BUFFER_SIZE];
    memset(&buffer, 0, sizeof(buffer));

    if (signal(SIGINT, sigint_handler) != 0)
    {
        printf("[ERROR] signal initial\n");
    }

    client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_fd <= 0)
    {
        printf("[ERROR] socket initial\n");
        return 1;
    }
    
    /* Configure Unix Domain Socket address */
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);
    printf("[monitor-cli] Connected to /tmp/monitor.sock\n");
    return_value = connect(client_fd, (struct sockaddr*) &addr, sizeof(struct sockaddr_un));
    if(return_value != 0)
    {
        printf("[ERROR] connect error\n");
        return 1;
    }

    while(1)
    {
        fflush(stdin);
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0';
        return_value = send(client_fd, buffer, BUFFER_SIZE, 0);
        if (return_value == -1)
        {
            printf("[ERROR] send error\n");
            return 1;
        }
        printf("[monitor-cli] Send command\n");
        
        if(strcmp(buffer, "cpu") == 0)
        {
            char response[BUFFER_SIZE];
            return_value = recv(client_fd, response, sizeof(response), 0);
            if (return_value == -1)
            {
                printf("[ERROR] recv error\n");
                return 1;
            }
            printf("%s\n", response);
            continue;
        }
        else if (strcmp(buffer, "mem") == 0)
        {
            char response[BUFFER_SIZE];
            return_value = recv(client_fd, response, sizeof(response), 0);
            if (return_value == -1)
            {
                printf("[ERROR] recv error\n");
                return 1;
            }
            printf("%s\n", response);
            continue;
        } 
        else if(strcmp(buffer, "quit") == 0)
        {
            break;
        }
        return_value = recv(client_fd, &buffer_recv, sizeof(buffer_recv), 0);
        if (return_value == -1)
        {
            printf("[ERROR] recv error\n");
            return 1;
        }
        printf("%s\n", buffer_recv);
    }

    close(client_fd); 
    printf("[monitor-cli] Closed\n");
    return 0;
}