#ifndef CHAT_SERVER_H
#define CHAT_SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/file.h>
#include <crypt.h>

#define PORT 9000
#define MAX_CLIENTS 100
#define BUFFER_SIZE 2048
#define USERNAME_LEN 64
#define PASSWORD_LEN 64
#define SALT "$6$devlinuxsalt$"

#define USER_DB_FILE "users.db"
#define HISTORY_FILE "chat_history.txt"

typedef struct {
    int fd;
    int authenticated;
    char username[USERNAME_LEN];
} client_t;

extern client_t *clients[MAX_CLIENTS];
extern pthread_mutex_t clients_mutex;

// Functions for Auth
int auth_register(const char *username, const char *password);
int auth_login(const char *username, const char *password);

// Functions for Broadcast
void broadcast_message(const char *sender, const char *message);
void broadcast_system(const char *message);
void send_online_list(int client_fd);

// Functions for History
void history_append(const char *sender, const char *message);
void history_send_all(int client_fd);

#endif // CHAT_SERVER_H