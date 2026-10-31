#include "chat_server.h"
#include <time.h>

void history_append(const char *sender, const char *message) {
    FILE *fp = fopen(HISTORY_FILE, "a");
    if (!fp) return;

    int fd = fileno(fp);
    flock(fd, LOCK_EX);

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char time_str[16];
    strftime(time_str, sizeof(time_str), "%H:%M:%S", t);

    fprintf(fp, "[%s] %s: %s\n", time_str, sender, message);
    fflush(fp);

    flock(fd, LOCK_UN);
    fclose(fp);
}

void history_send_all(int client_fd) {
    FILE *fp = fopen(HISTORY_FILE, "r");
    if (!fp) return;

    int fd = fileno(fp);
    flock(fd, LOCK_SH);

    char line[BUFFER_SIZE];
    char packet[BUFFER_SIZE + 32];

    send(client_fd, "HIST_START\n", 11, 0);
    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\r\n")] = 0;
        snprintf(packet, sizeof(packet), "HIST_ITEM %s\n", line);
        send(client_fd, packet, strlen(packet), 0);
    }
    send(client_fd, "HIST_END\n", 9, 0);

    flock(fd, LOCK_UN);
    fclose(fp);
}