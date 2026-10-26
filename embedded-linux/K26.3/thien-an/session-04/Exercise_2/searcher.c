#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_LINE 256

int main(int argc, char *argv[]) {
	// Receive argv[1] = student ID, argv[2] = data file path
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <student_id> <file_path>\n", argv[0]);
		exit(2);
	}

	printf("[SEARCHER] PID: %d | PPID: %d\n", getpid(), getppid());
	printf("[SEARCHER] Searching for \"%s\" in %s...\n", argv[1], argv[2]);

	FILE *file = fopen(argv[2], "r");
	if (!file) {
		// File or argument error -> perror and exit(2)
		perror("Error opening file");
		exit(2);
	}

	char line[MAX_LINE];
	while (fgets(line, sizeof(line), file)) {
		// Remove newline character if present
		line[strcspn(line, "\n")] = 0;

		// Use strtok to split fields by pipe '|'[cite: 2]
		char *id = strtok(line, "|");
		char *name = strtok(NULL, "|");
		char *cls = strtok(NULL, "|");
		char *gpa_str = strtok(NULL, "|");

		if (id && strcmp(id, argv[1]) == 0) {
			float gpa = atof(gpa_str);
			const char *grade = "Poor";
			
			// Grade classification by GPA[cite: 2]
			if (gpa >= 8.5) grade = "Excellent";
			else if (gpa >= 7.0) grade = "Good";
			else if (gpa >= 5.0) grade = "Average";

			// Print full record + grade classification[cite: 2]
			printf("\n========== SEARCH RESULT ==========\n");
			printf("  ID      : %s\n", id);
			printf("  Name    : %s\n", name);
			printf("  Class   : %s\n", cls);
			printf("  GPA     : %.1f\n", gpa);
			printf("  Grade   : %s\n", grade);
			printf("====================================\n\n");
			
			fclose(file);
			exit(0); // Exit code 0: Student found[cite: 2]
		}
	}

	// Not found -> print message and exit(1)[cite: 2]
	printf("[SEARCHER] No student found with ID: %s\n\n", argv[1]);
	fclose(file);
	exit(1);
}