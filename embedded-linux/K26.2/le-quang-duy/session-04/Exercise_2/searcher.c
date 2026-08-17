#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <student_id> <data_file>\n", argv[0]);
        exit(2); // Error code
    }

    char *target_id = argv[1];
    char *filename = argv[2];

    printf("[SEARCHER] PID: %d | PPID: %d\n", getpid(), getppid());
    printf("[SEARCHER] Searching for \"%s\" in %s...\n\n", target_id, filename);

    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Cannot open file");
        exit(2);
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0; // Strip newline

        // Parse line (dùng strtok chia chuỗi bởi dấu |)
        char *id = strtok(line, "|");
        char *name = strtok(NULL, "|");
        char *cls = strtok(NULL, "|");
        char *gpa_str = strtok(NULL, "|");

        if (id != NULL && strcmp(id, target_id) == 0) {
            float gpa = atof(gpa_str);
            const char *grade = "Poor"; // < 5.0
            
            if (gpa >= 8.5) grade = "Excellent";
            else if (gpa >= 7.0) grade = "Good";
            else if (gpa >= 5.0) grade = "Average";

            printf("========== SEARCH RESULT ==========\n");
            printf("  ID      : %s\n", id);
            printf("  Name    : %s\n", name);
            printf("  Class   : %s\n", cls);
            printf("  GPA     : %s\n", gpa_str);
            printf("  Grade   : %s\n", grade);
            printf("====================================\n");

            fclose(file);
            exit(0); // Thành công, tìm thấy
        }
    }

    printf("[SEARCHER] No student found with ID: %s\n", target_id);
    fclose(file);
    exit(1); // Không tìm thấy
}