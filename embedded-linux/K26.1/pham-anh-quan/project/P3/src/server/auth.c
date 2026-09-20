#include "chat_server.h"

int auth_register(const char *username, const char *password) {
    if (strlen(username) == 0 || strlen(password) == 0) return 0;

    FILE *fp = fopen(USER_DB_FILE, "a+");
    if (!fp) return 0;

    int fd = fileno(fp);
    flock(fd, LOCK_EX);

    char u[USERNAME_LEN], h[256];
    fseek(fp, 0, SEEK_SET);
    while (fscanf(fp, "%63s %255s", u, h) == 2) {
        if (strcmp(u, username) == 0) {
            flock(fd, LOCK_UN);
            fclose(fp);
            return 0; // User đã tồn tại
        }
    }

    char *hashed_pw = crypt(password, SALT);
    fprintf(fp, "%s %s\n", username, hashed_pw);
    fflush(fp);

    flock(fd, LOCK_UN);
    fclose(fp);
    return 1;
}

int auth_login(const char *username, const char *password) {
    FILE *fp = fopen(USER_DB_FILE, "r");
    if (!fp) return 0;

    int fd = fileno(fp);
    flock(fd, LOCK_SH);

    char u[USERNAME_LEN], h[256];
    int success = 0;

    while (fscanf(fp, "%63s %255s", u, h) == 2) {
        if (strcmp(u, username) == 0) {
            char *hashed_input = crypt(password, h);
            if (hashed_input && strcmp(hashed_input, h) == 0) {
                success = 1;
            }
            break;
        }
    }

    flock(fd, LOCK_UN);
    fclose(fp);
    return success;
}