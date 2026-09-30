#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define FILE_NAME "students.dat"
#define STUDENT_NAME_MAX 64

typedef struct {
	int   id;
	char  name[STUDENT_NAME_MAX];
	int   age;
	float gpa;
} Student;

void add_student() {
	Student s;
	printf("Enter ID: ");
	scanf("%d", &s.id);
	printf("Enter Name: ");
	scanf(" %[^\n]", s.name); 
	printf("Enter Age: ");
	scanf("%d", &s.age);
	printf("Enter GPA: ");
	scanf("%f", &s.gpa);

	int fd = open(FILE_NAME, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd < 0) {
		perror("Error opening file");
		return;
	}

	ssize_t written = write(fd, &s, sizeof(Student));
	if (written != sizeof(Student)) {
		perror("Error writing to file");
	} else {
		printf("Student added successfully.\n");
	}
	
	close(fd);
}

void list_students() {
	int fd = open(FILE_NAME, O_RDONLY);
	if (fd < 0) {
		printf("No students data found (or cannot open file).\n");
		return;
	}

	Student s;
	int count = 0;
	ssize_t bytes_read;
	
	printf("\n--- List of Students ---\n");
	
	while ((bytes_read = read(fd, &s, sizeof(Student))) > 0) {
		if (bytes_read != sizeof(Student)) {
			printf("Warning: Partial data read.\n");
			break;
		}
		printf("ID: %d | Name: %s | Age: %d | GPA: %.2f\n", s.id, s.name, s.age, s.gpa);
		count++;
	}
	
	if (bytes_read < 0) {
		perror("read error");
	}
	
	if (count == 0) {
		printf("The list is empty.\n");
	}
	
	close(fd);
}

void find_student() {
	int target_id;
	printf("Enter Student ID to find: ");
	scanf("%d", &target_id);

	int fd = open(FILE_NAME, O_RDONLY);
	if (fd < 0) {
		perror("Error opening file");
		return;
	}

	Student s;
	int found = 0;
	ssize_t bytes_read;
	
	while ((bytes_read = read(fd, &s, sizeof(Student))) > 0) {
		if (bytes_read != sizeof(Student)) {
			break;
		}
		if (s.id == target_id) {
			printf("\nStudent found:\n");
			printf("ID: %d | Name: %s | Age: %d | GPA: %.2f\n", s.id, s.name, s.age, s.gpa);
			found = 1;
			break; 
		}
	}
	
	if (bytes_read < 0) {
		perror("read error");
	}
	
	if (!found) {
		printf("Student with ID %d not found.\n", target_id);
	}
	
	close(fd);
}

int main() {
	int choice;
	
	while (1) {
		printf("\n1. Add student\n");
		printf("2. List all students\n");
		printf("3. Find student by ID\n");
		printf("4. Exit\n");
		printf("Choice: ");
		
		if (scanf("%d", &choice) != 1) {
			while (getchar() != '\n');
			printf("Invalid input. Please enter a number.\n");
			continue;
		}

		switch (choice) {
			case 1: 
				add_student(); 
				break;
			case 2: 
				list_students(); 
				break;
			case 3: 
				find_student(); 
				break;
			case 4:
				printf("Exiting...\n");
				return 0;
			default:
				printf("Invalid choice. Please enter from 1 - 4\n");
		}
	}
	return 0;
}