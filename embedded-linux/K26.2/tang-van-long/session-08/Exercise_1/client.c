#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/monitor.sock"
#define BUFFER_SIZE 256

static void trim_newline(char *s)
{
    size_t len = strlen(s);
    while (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r')) {
        s[--len] = '\0';
    }
}

int main(void)
{
    struct sockaddr_un addr;

    int sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (connect(sock_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("connect");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    printf("[monitor-cli] Connected to %s\n", SOCKET_PATH);

    char line[BUFFER_SIZE];
    char response[BUFFER_SIZE];

    while (1) {
        printf("> ");
        fflush(stdout);

        if (fgets(line, sizeof(line), stdin) == NULL) {
            printf("\n[monitor-cli] EOF on stdin, closing connection.\n");
            break;
        }

        trim_newline(line);

        if (strlen(line) == 0) {
            continue; 
        }

        if (strcmp(line, "quit") == 0) {
            break;
        }

        ssize_t sent = send(sock_fd, line, strlen(line), 0);
        if (sent == -1) {
            perror("send");
            break;
        }

        memset(response, 0, sizeof(response));
        ssize_t n = recv(sock_fd, response, sizeof(response) - 1, 0);
        if (n == -1) {
            perror("recv");
            break;
        }
        if (n == 0) {
            printf("[monitor-cli] Server closed the connection.\n");
            break;
        }

        response[n] = '\0';
        printf("%s\n", response);
    }

    close(sock_fd);
    return 0;
}