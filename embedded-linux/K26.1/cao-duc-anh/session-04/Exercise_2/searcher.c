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

    /* Block buffering: read 4KB at a time, accumulate one line at a time.
     * Handles files of any size — RAM usage stays constant (blk + line). */
    char    blk[4096];
    char    line[256];
    int     lpos = 0;
    ssize_t n;

    while ((n = read(fd, blk, sizeof(blk))) > 0) {
        for (ssize_t i = 0; i < n; i++) {
            if (blk[i] == '\n') {
                line[lpos] = '\0';
                lpos = 0;

                /* Split fields by '|' */
                char *fields[4] = {NULL};
                int   nf = 0;
                char *tok = strtok(line, "|");
                while (tok && nf < 4) {
                    fields[nf++] = tok;
                    tok = strtok(NULL, "|");
                }

                if (nf == 4 && strcmp(fields[0], argv[1]) == 0) {
                    float gpa = atof(fields[3]);
                    close(fd);
                    printf("========== SEARCH RESULT ==========\n");
                    printf("  ID      : %s\n", fields[0]);
                    printf("  Name    : %s\n", fields[1]);
                    printf("  Class   : %s\n", fields[2]);
                    printf("  GPA     : %.1f\n", gpa);
                    printf("  Grade   : %s\n",  classify(gpa));
                    printf("====================================\n\n");
                    exit(0);
                }
            } else if (lpos < (int)sizeof(line) - 1) {
                line[lpos++] = blk[i];
            }
        }
    }
    close(fd);

    printf("[SEARCHER] No student found with ID: %s\n\n", argv[1]);
    exit(1);
}
