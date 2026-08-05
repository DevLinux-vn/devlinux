#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH        "/tmp/monitor.sock"

#define BUFFER_SIZE        256U

#define QUIT_COMMAND       "quit"

/*------------------------------------------------------------------*/

static int create_client_socket(void)
{
    int client_fd;
    struct sockaddr_un server_addr;

    client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_fd == -1)
    {
        perror("socket");
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;

    strncpy(server_addr.sun_path,
            SOCKET_PATH,
            sizeof(server_addr.sun_path) - 1);

    if (connect(client_fd,
                (struct sockaddr *)&server_addr,
                sizeof(server_addr)) == -1)
    {
        perror("connect");
        close(client_fd);
        return -1;
    }

    return client_fd;
}

/*------------------------------------------------------------------*/

static int send_command(int client_fd, const char *command)
{
    ssize_t sent_bytes;

    sent_bytes = send(client_fd,
                      command,
                      strlen(command),
                      0);

    if (sent_bytes == -1)
    {
        perror("send");
        return -1;
    }

    return 0;
}

/*------------------------------------------------------------------*/

static int receive_response(int client_fd)
{
    char response[BUFFER_SIZE];
    ssize_t received_bytes;

    memset(response, 0, sizeof(response));

    received_bytes = recv(client_fd,
                          response,
                          sizeof(response) - 1,
                          0);

    if (received_bytes == -1)
    {
        perror("recv");
        return -1;
    }

    if (received_bytes == 0)
    {
        printf("Server disconnected.\n");
        return -1;
    }

    response[received_bytes] = '\0';

    printf("%s\n", response);

    return 0;
}

/*------------------------------------------------------------------*/

static void command_loop(int client_fd)
{
    char command[BUFFER_SIZE];

    while (1)
    {
        printf("> ");

        if (fgets(command, sizeof(command), stdin) == NULL)
        {
            break;
        }

        command[strcspn(command, "\r\n")] = '\0';

        if (strlen(command) == 0U)
        {
            continue;
        }

        if (strcmp(command, QUIT_COMMAND) == 0)
        {
            break;
        }

        if (send_command(client_fd, command) == -1)
        {
            break;
        }

        if (receive_response(client_fd) == -1)
        {
            break;
        }
    }
}

/*------------------------------------------------------------------*/

int main(void)
{
    int client_fd;

    client_fd = create_client_socket();
    if (client_fd == -1)
    {
        return EXIT_FAILURE;
    }

    printf("[monitor-cli] Connected to %s\n", SOCKET_PATH);

    command_loop(client_fd);

    if (close(client_fd) == -1)
    {
        perror("close");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}