#include "chat_server.h"

client_t *clients[MAX_CLIENTS] = {NULL};
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void *client_handler(void *arg) {
    client_t *cli = (client_t *)arg;
    char buffer[BUFFER_SIZE];
    int fail_attempts = 0;

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(cli->fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) break; // Client disconnect hoặc lỗi

        buffer[strcspn(buffer, "\r\n")] = 0;
        if (strlen(buffer) == 0) continue;

        char cmd[32], arg1[USERNAME_LEN], arg2[PASSWORD_LEN];
        memset(cmd, 0, sizeof(cmd));
        memset(arg1, 0, sizeof(arg1));
        memset(arg2, 0, sizeof(arg2));

        sscanf(buffer, "%31s %63s %63s", cmd, arg1, arg2);

        if (!cli->authenticated) {
            if (strcmp(cmd, "REGISTER") == 0) {
                if (auth_register(arg1, arg2)) {
                    send(cli->fd, "AUTH_OK Registered successfully\n", 31, 0);
                } else {
                    send(cli->fd, "AUTH_FAIL Registration failed\n", 30, 0);
                }
            } else if (strcmp(cmd, "LOGIN") == 0) {
                if (auth_login(arg1, arg2)) {
                    cli->authenticated = 1;
                    strncpy(cli->username, arg1, USERNAME_LEN - 1);
                    send(cli->fd, "AUTH_OK Login successful\n", 25, 0);
                    
                    history_send_all(cli->fd); // Gửi lịch sử khi join
                    
                    char sys_msg[128];
                    snprintf(sys_msg, sizeof(sys_msg), "*** %s has joined ***", cli->username);
                    broadcast_system(sys_msg);
                } else {
                    fail_attempts++;
                    send(cli->fd, "AUTH_FAIL Invalid credentials\n", 30, 0);
                    if (fail_attempts >= 3) {
                        break; // Đóng kết nối nếu sai 3 lần
                    }
                }
            } else {
                send(cli->fd, "ERR Invalid command\n", 20, 0);
            }
        } else {
            // Khi đã authenticated
            if (strcmp(cmd, "MSG") == 0) {
                char *msg_ptr = strstr(buffer, " ");
                if (msg_ptr) {
                    msg_ptr++;
                    broadcast_message(cli->username, msg_ptr);
                }
            } else if (strcmp(cmd, "WHO") == 0) {
                send_online_list(cli->fd);
            } else if (strcmp(cmd, "QUIT") == 0) {
                break;
            }
        }
    }

    // Cleanup khi client ngắt kết nối
    close(cli->fd);
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] == cli) {
            clients[i] = NULL;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    if (cli->authenticated) {
        char sys_msg[128];
        snprintf(sys_msg, sizeof(sys_msg), "*** %s has left ***", cli->username);
        broadcast_system(sys_msg);
    }

    free(cli);
    pthread_exit(NULL);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 10) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("[SERVER] Running on port %d...\n", PORT);

    while (1) {
        socklen_t addrlen = sizeof(address);
        new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
        if (new_socket < 0) continue;

        pthread_mutex_lock(&clients_mutex);
        int i;
        for (i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i] == NULL) {
                client_t *cli = (client_t *)malloc(sizeof(client_t));
                cli->fd = new_socket;
                cli->authenticated = 0;
                memset(cli->username, 0, USERNAME_LEN);
                clients[i] = cli;

                pthread_t tid;
                pthread_create(&tid, NULL, client_handler, (void *)cli);
                pthread_detach(tid);
                break;
            }
        }
        pthread_mutex_unlock(&clients_mutex);

        if (i == MAX_CLIENTS) {
            printf("[SERVER] Max clients reached. Rejecting connection.\n");
            close(new_socket);
        }
    }

    close(server_fd);
    return 0;
}