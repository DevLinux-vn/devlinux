#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Grade classification function
const char* classify_grade(float gpa) {
    if (gpa >= 8.5) return "Excellent";
    if (gpa >= 7.0) return "Good";
    if (gpa >= 5.0) return "Average";
    return "Poor";
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "[SEARCHER] Usage: %s <student_id> <data_file>\n", argv[0]);
        exit(2);
    }
    
    char *search_id = argv[1];
    char *filename = argv[2];
    
    printf("[SEARCHER] PID: %d | PPID: %d\n", getpid(), getppid());
    printf("[SEARCHER] Searching for \"%s\" in %s...\n\n", search_id, filename);
    
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("[SEARCHER] fopen");
        exit(2);
    }
    
    char line[256];
    int found = 0;
    
    while (fgets(line, sizeof(line), file) != NULL) {
        // Remove newline
        line[strcspn(line, "\n")] = '\0';
        
        // Parse line using strtok
        char line_copy[256];
        strcpy(line_copy, line);
        
        char *id = strtok(line_copy, "|");
        char *name = strtok(NULL, "|");
        char *class = strtok(NULL, "|");
        char *gpa_str = strtok(NULL, "|");
        
        if (id == NULL || name == NULL || class == NULL || gpa_str == NULL) {
            continue;  // Skip malformed lines
        }
        
        if (strcmp(id, search_id) == 0) {
            float gpa = atof(gpa_str);
            const char *grade = classify_grade(gpa);
            
            printf("========== SEARCH RESULT ==========\n");
            printf("  ID      : %s\n", id);
            printf("  Name    : %s\n", name);
            printf("  Class   : %s\n", class);
            printf("  GPA     : %.1f\n", gpa);
            printf("  Grade   : %s\n", grade);
            printf("====================================\n");
            
            found = 1;
            break;
        }
    }
    
    fclose(file);
    
    if (!found) {
        printf("[SEARCHER] No student found with ID: %s\n", search_id);
        exit(1);
    }
    
    exit(0);
}
