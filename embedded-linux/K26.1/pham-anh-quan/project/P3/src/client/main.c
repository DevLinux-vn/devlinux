#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT 9000
#define BUFFER_SIZE 2048

int sock = 0;
int running = 1;

void *receive_handler(void *arg) {
    (void)arg;
    char buffer[BUFFER_SIZE];

    while (running) {
        memset(buffer, 0, sizeof(buffer));
        int bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0) {
            if (running) {
                printf("\n[!] Mất kết nối tới server.\n");
            }
            running = 0;
            exit(0);
        }

        buffer[strcspn(buffer, "\r\n")] = 0;

        if (strncmp(buffer, "MSG ", 4) == 0) {
            char sender[64], msg[BUFFER_SIZE];
            sscanf(buffer + 4, "%63s %[^\n]", sender, msg);
            printf("\r\33[2K%s: %s\n> ", sender, msg);
            fflush(stdout);
        } else if (strncmp(buffer, "SYS ", 4) == 0) {
            printf("\r\33[2K%s\n> ", buffer + 4);
            fflush(stdout);
        } else if (strncmp(buffer, "HIST_ITEM ", 10) == 0) {
            printf("%s\n", buffer + 10);
        } else if (strcmp(buffer, "HIST_START") == 0) {
            printf("--- Lịch sử tin nhắn ---\n");
        } else if (strcmp(buffer, "HIST_END") == 0) {
            printf("--------------------------------------\n> ");
            fflush(stdout);
        } else if (strncmp(buffer, "WHO_REPLY", 9) == 0) {
            printf("\r\33[2K[Online Users]: %s\n> ", buffer + 10);
            fflush(stdout);
        }
    }
    return NULL;
}

int main() {
    struct sockaddr_in serv_addr;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Error creating socket \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address\n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed\n");
        return -1;
    }

    printf("=== DevLinux Chat Client ===\n");
    printf("Server: %s:%d\n", SERVER_IP, PORT);

    char buffer[BUFFER_SIZE];
    int auth_success = 0;
    int attempts = 0;

    while (attempts < 3 && !auth_success) {
        char choice[10], username[64], password[64];
        printf("Đăng nhập hay Đăng ký? [login/register]: ");
        if (!fgets(choice, sizeof(choice), stdin)) break;
        choice[strcspn(choice, "\r\n")] = 0;

        if (strcmp(choice, "login") != 0 && strcmp(choice, "register") != 0) {
            printf("[!] Lựa chọn không hợp lệ.\n");
            continue;
        }

        printf("Username: ");
        if (!fgets(username, sizeof(username), stdin)) break;
        username[strcspn(username, "\r\n")] = 0;

        printf("Password: ");
        if (!fgets(password, sizeof(password), stdin)) break;
        password[strcspn(password, "\r\n")] = 0;

        if (strcmp(choice, "login") == 0) {
            snprintf(buffer, sizeof(buffer), "LOGIN %s %s\n", username, password);
        } else {
            snprintf(buffer, sizeof(buffer), "REGISTER %s %s\n", username, password);
        }

        send(sock, buffer, strlen(buffer), 0);

        memset(buffer, 0, sizeof(buffer));
        recv(sock, buffer, sizeof(buffer) - 1, 0);

        if (strncmp(buffer, "AUTH_OK", 7) == 0) {
            auth_success = 1;
            printf("[+] Auth thành công!\n");
        } else {
            attempts++;
            printf("[!] Sai username hoặc password. Vui lòng thử lại (%d/3).\n", attempts);
        }
    }

    if (!auth_success) {
        printf("[!] Đã nhập sai quá số lần cho phép. Kết nối đã bị đóng.\n");
        close(sock);
        return 0;
    }

    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, receive_handler, NULL);

    char input[BUFFER_SIZE];
    while (running) {
        if (!fgets(input, sizeof(input), stdin)) break;
        input[strcspn(input, "\r\n")] = 0;

        if (strlen(input) == 0) continue;

        if (strcmp(input, "/quit") == 0) {
            send(sock, "QUIT\n", 5, 0);
            running = 0;
            break;
        } else if (strcmp(input, "/who") == 0) {
            send(sock, "WHO\n", 4, 0);
        } else if (strcmp(input, "/help") == 0) {
            printf("[Help]: /who - Liệt kê user online | /quit - Thoát chương trình\n> ");
            fflush(stdout);
        } else {
            snprintf(buffer, sizeof(buffer), "MSG %s\n", input);
            send(sock, buffer, strlen(buffer), 0);
        }
    }

    close(sock);
    return 0;
}