#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static const char *grade_of(float gpa) {
    if (gpa >= 8.5) return "Excellent";
    if (gpa >= 7.0) return "Good";
    if (gpa >= 5.0) return "Average";
    return "Poor";
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <student_id> <data_file>\n", argv[0]);
        exit(2);
    }

    const char *target_id = argv[1];
    const char *data_file = argv[2];

    printf("[SEARCHER] PID: %d | PPID: %d\n", getpid(), getppid());
    printf("[SEARCHER] Searching for \"%s\" in %s...\n", target_id, data_file);

    FILE *fp = fopen(data_file, "r");
    if (fp == NULL) {
        perror("[SEARCHER] fopen");
        exit(2);
    }

    char line[256];
    while (fgets(line, sizeof(line), fp) != NULL) {
        line[strcspn(line, "\r\n")] = '\0';

        char *id    = strtok(line, "|");
        char *name  = strtok(NULL, "|");
        char *class = strtok(NULL, "|");
        char *gpa_s = strtok(NULL, "|");

        if (id == NULL || name == NULL || class == NULL || gpa_s == NULL) {
            continue; // dòng dữ liệu bị lỗi định dạng, bỏ qua
        }

        if (strcmp(id, target_id) == 0) {
            float gpa = atof(gpa_s);
            printf("\n========== SEARCH RESULT ==========\n");
            printf("  ID      : %s\n", id);
            printf("  Name    : %s\n", name);
            printf("  Class   : %s\n", class);
            printf("  GPA     : %.1f\n", gpa);
            printf("  Grade   : %s\n", grade_of(gpa));
            printf("====================================\n\n");
            fclose(fp);
            exit(0);
        }
    }

    fclose(fp);
    printf("[SEARCHER] No student found with ID: %s\n", target_id);
    exit(1);
}
