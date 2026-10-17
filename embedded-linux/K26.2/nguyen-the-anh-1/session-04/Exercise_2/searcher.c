#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Phan loai xep hang theo GPA */
static const char *classify(float gpa)
{
    if (gpa >= 8.5f) return "Excellent";
    if (gpa >= 7.0f) return "Good";
    if (gpa >= 5.0f) return "Average";
    return "Poor";
}

int main(int argc, char *argv[])
{
    /* argv[1] = student ID can tim, argv[2] = duong dan file du lieu */
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <student_id> <data_file>\n", argv[0]);
        exit(2);
    }

    const char *target_id = argv[1];
    const char *data_file = argv[2];

    printf("[SEARCHER] PID: %d | PPID: %d\n", getpid(), getppid());
    printf("[SEARCHER] Searching for \"%s\" in %s...\n", target_id, data_file);
    fflush(stdout);

    FILE *fp = fopen(data_file, "r");
    if (fp == NULL) {
        perror("fopen");
        exit(2);
    }

    char line[256];
    while (fgets(line, sizeof(line), fp) != NULL) {
        /* Xoa newline cuoi dong neu co */
        line[strcspn(line, "\r\n")] = '\0';

        /* Copy ra buffer rieng vi strtok se pha huy chuoi goc */
        char buf[256];
        strncpy(buf, line, sizeof(buf) - 1);
        buf[sizeof(buf) - 1] = '\0';

        char *id    = strtok(buf, "|");
        char *name  = strtok(NULL, "|");
        char *cls   = strtok(NULL, "|");
        char *gpa_s = strtok(NULL, "|");

        if (id == NULL || name == NULL || cls == NULL || gpa_s == NULL) {
            continue;   /* dong loi dinh dang, bo qua */
        }

        if (strcmp(id, target_id) == 0) {
            float gpa = atof(gpa_s);
            printf("========== SEARCH RESULT ==========\n");
            printf("  ID      : %s\n", id);
            printf("  Name    : %s\n", name);
            printf("  Class   : %s\n", cls);
            printf("  GPA     : %.1f\n", gpa);
            printf("  Grade   : %s\n", classify(gpa));
            printf("====================================\n");
            fclose(fp);
            exit(0);
        }
    }

    printf("[SEARCHER] No student found with ID: %s\n", target_id);
    fclose(fp);
    exit(1);
}
