#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

// Environment variables array for execve[cite: 2]
extern char **environ;

int main() {
	char student_id[64];

	printf("=============================================\n");
	printf("   STUDENT LOOKUP SYSTEM — MANAGER\n");
	printf("   (fork + execve | file: students.txt)\n");
	printf("=============================================\n");
	printf("[MANAGER] PID: %d\n", getpid());
	printf("Enter student ID ('quit' to exit).\n");
	
	while (1) {
		printf("\n---------------------------------------------\n");
		printf("Student ID: ");
		
		if (scanf(" %63[^\n]", student_id) != 1) {
			while (getchar() != '\n');
			continue;
		}

		if (strcmp(student_id, "quit") == 0) {
			printf("[MANAGER] Exiting. Goodbye!\n");
			break;
		}

		pid_t pid = fork();

		if (pid < 0) {
			perror("fork failed");
			continue;
		} else if (pid == 0) {
			// [Child] Prepare arguments for execve[cite: 2]
			char *args[] = {"./searcher", student_id, "students.txt", NULL};
			
			// Replace child process image with searcher program[cite: 2]
			execve(args[0], args, environ);
			
			// This line is normally never reached because execve replaces the program code.
			// It only executes if execve FAILS (e.g., file not found or no execute permission)[cite: 2]
			perror("execve failed");
			exit(2);
		} else {
			// [Parent]
			printf("\n[MANAGER] fork() → child PID: %d\n", pid);
			printf("[MANAGER] Waiting for child (waitpid)...\n\n");
			
			int status;
			waitpid(pid, &status, 0); // Read the outcome from child[cite: 2]
			
			if (WIFEXITED(status)) {
				// Read exit code via WEXITSTATUS[cite: 2]
				int exit_code = WEXITSTATUS(status);
				const char *meaning = "Unknown";
				
				if (exit_code == 0) meaning = "Found";
				else if (exit_code == 1) meaning = "Not found";
				else if (exit_code == 2) meaning = "Error";
				
				printf("[MANAGER] Child (PID %d) exited. code=%d → %s\n", pid, exit_code, meaning);
			} else {
				printf("[MANAGER] Child (PID %d) terminated abnormally.\n", pid);
			}
		}
	}
	
	return 0;
}