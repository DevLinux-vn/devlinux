#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static const char *classify(float gpa)
{
    if (gpa >= 8.5f) return "Excellent";
    if (gpa >= 7.0f) return "Good";
    if (gpa >= 5.0f) return "Average";
    return "Poor";
}

int main(int argc, char *argv[])
{
    if (argc < 3) {
        write(STDERR_FILENO, "Usage: searcher <student_id> <file>\n", 36);
        exit(2);
    }

    printf("[SEARCHER] PID: %d | PPID: %d\n", getpid(), getppid());
    printf("[SEARCHER] Searching for \"%s\" in %s...\n\n", argv[1], argv[2]);

    int fd = open(argv[2], O_RDONLY);
    if (fd < 0) {
        perror("open");
        exit(2);
    }

    /* Read entire file into buffer using syscall */
    char buf[4096];
    ssize_t total = 0, n;
    while ((n = read(fd, buf + total, sizeof(buf) - 1 - total)) > 0)
        total += n;
    close(fd);
    buf[total] = '\0';

    /* Parse line by line */
    char *line = buf;
    while (line && *line) {
        char *next = strchr(line, '\n');
        if (next) *next = '\0';

        /* Split fields by '|' */
        char tmp[256];
        strncpy(tmp, line, sizeof(tmp) - 1);
        tmp[sizeof(tmp) - 1] = '\0';

        char *fields[4] = {NULL};
        int  nf = 0;
        char *tok = strtok(tmp, "|");
        while (tok && nf < 4) {
            fields[nf++] = tok;
            tok = strtok(NULL, "|");
        }

        if (nf == 4 && strcmp(fields[0], argv[1]) == 0) {
            float gpa = atof(fields[3]);
            printf("========== SEARCH RESULT ==========\n");
            printf("  ID      : %s\n", fields[0]);
            printf("  Name    : %s\n", fields[1]);
            printf("  Class   : %s\n", fields[2]);
            printf("  GPA     : %.1f\n", gpa);
            printf("  Grade   : %s\n",  classify(gpa));
            printf("====================================\n\n");
            exit(0);
        }

        line = next ? next + 1 : NULL;
    }

    printf("[SEARCHER] No student found with ID: %s\n\n", argv[1]);
    exit(1);
}
