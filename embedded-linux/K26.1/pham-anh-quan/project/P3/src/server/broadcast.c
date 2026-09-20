#include "chat_server.h"

void broadcast_message(const char *sender, const char *message) {
    char packet[BUFFER_SIZE];
    snprintf(packet, sizeof(packet), "MSG %s %s\n", sender, message);

    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] && clients[i]->authenticated) {
            send(clients[i]->fd, packet, strlen(packet), 0);
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    history_append(sender, message);
}

void broadcast_system(const char *message) {
    char packet[BUFFER_SIZE];
    snprintf(packet, sizeof(packet), "SYS %s\n", message);

    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] && clients[i]->authenticated) {
            send(clients[i]->fd, packet, strlen(packet), 0);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void send_online_list(int client_fd) {
    char response[BUFFER_SIZE] = "WHO_REPLY";

    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] && clients[i]->authenticated) {
            strcat(response, " ");
            strcat(response, clients[i]->username);
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    strcat(response, "\n");
    send(client_fd, response, strlen(response), 0);
}