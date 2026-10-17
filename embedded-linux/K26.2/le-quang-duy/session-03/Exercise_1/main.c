#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

typedef struct {
    int   id;
    char  name[64];
    int   age;
    float gpa;
} Student;

void clear_stdin() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main() {
    int fd = open("students.dat", O_RDWR | O_CREAT, 0644);
    if (fd == -1) {
        perror("Cannot open file");
        return 1;
    }

    int choice;
    do {
        printf("\n--- STUDENT MANAGEMENT ---\n");
        printf("1. Add student\n");
        printf("2. List all students\n");
        printf("3. Find student by ID\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        clear_stdin();

        if (choice == 1) {
            Student s;
            printf("Enter ID: ");
            scanf("%d", &s.id);
            clear_stdin();
            
            printf("Enter Name: ");
            fgets(s.name, 64, stdin);
            s.name[strcspn(s.name, "\n")] = 0; // Remove trailing newline
            
            printf("Enter Age: ");
            scanf("%d", &s.age);
            
            printf("Enter GPA: ");
            scanf("%f", &s.gpa);
            
            lseek(fd, 0, SEEK_END);
            if (write(fd, &s, sizeof(Student)) != sizeof(Student)) {
                perror("Failed to write to file");
            } else {
                printf("Student added successfully.\n");
            }
        } 
        else if (choice == 2) {
            Student s;
            lseek(fd, 0, SEEK_SET);
            int count = 0;
            printf("\n--- Student List ---\n");
            while (read(fd, &s, sizeof(Student)) == sizeof(Student)) {
                printf("ID: %d | Name: %s | Age: %d | GPA: %.2f\n", s.id, s.name, s.age, s.gpa);
                count++;
            }
            if (count == 0) printf("No students found.\n");
        } 
        else if (choice == 3) {
            int target_id;
            printf("Enter student ID to find: ");
            scanf("%d", &target_id);
            
            Student s;
            int found = 0;
            lseek(fd, 0, SEEK_SET);
            while (read(fd, &s, sizeof(Student)) == sizeof(Student)) {
                if (s.id == target_id) {
                    printf("\nStudent found:\n");
                    printf("ID: %d | Name: %s | Age: %d | GPA: %.2f\n", s.id, s.name, s.age, s.gpa);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                printf("Student with ID %d not found.\n", target_id);
            }
        }
    } while (choice != 4);

    close(fd);
    printf("Exiting program.\n");
    return 0;
}