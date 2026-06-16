#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct {
    char id[20];
    char name[50];
    char class[20];
    float gpa;
} Student;

char* get_grade(float gpa) {
    if (gpa >= 8.5) return "Excellent";
    if (gpa >= 7.0) return "Good";
    if (gpa >= 5.0) return "Average";
    return "Poor";
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        perror("Usage: searcher <student_id> <filename>");
        exit(2);
    }
    
    char *target_id = argv[1];
    char *filename = argv[2];
    
    printf("[SEARCHER] PID: %d | PPID: %d\n", getpid(), getppid());
    printf("[SEARCHER] Searching for \"%s\" in %s...\n\n", target_id, filename);
    
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("fopen");
        exit(2);
    }
    
    char line[256];
    while (fgets(line, sizeof(line), file) != NULL) {
        // Remove newline
        line[strcspn(line, "\n")] = '\0';
        
        // Parse the line with pipe delimiter
        Student student;
        char *token;
        int field = 0;
        
        char line_copy[256];
        strcpy(line_copy, line);
        
        token = strtok(line_copy, "|");
        while (token != NULL && field < 4) {
            switch (field) {
                case 0:
                    strcpy(student.id, token);
                    break;
                case 1:
                    strcpy(student.name, token);
                    break;
                case 2:
                    strcpy(student.class, token);
                    break;
                case 3:
                    student.gpa = atof(token);
                    break;
            }
            token = strtok(NULL, "|");
            field++;
        }
        
        // Check if this is the student we're looking for
        if (strcmp(student.id, target_id) == 0) {
            printf("========== SEARCH RESULT ==========\n");
            printf("  ID      : %s\n", student.id);
            printf("  Name    : %s\n", student.name);
            printf("  Class   : %s\n", student.class);
            printf("  GPA     : %.1f\n", student.gpa);
            printf("  Grade   : %s\n", get_grade(student.gpa));
            printf("====================================\n");
            fclose(file);
            exit(0);
        }
    }
    
    // Student not found
    printf("[SEARCHER] No student found with ID: %s\n", target_id);
    fclose(file);
    exit(1);
}
