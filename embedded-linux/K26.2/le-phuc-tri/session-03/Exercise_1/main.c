#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct {
    int   id;
    char  name[64];
    int   age;
    float gpa;
} Student;

#define FILE_NAME "students.dat"

void add_student(int fd)
{
    Student student;

    printf("Enter ID: ");
    scanf("%d", &student.id);

    printf("Enter name: ");
    scanf(" %63[^\n]", student.name);

    printf("Enter age: ");
    scanf("%d", &student.age);

    printf("Enter GPA: ");
    scanf("%f", &student.gpa);

    /*
     * Move to the end of the file.
     */
    if (lseek(fd, 0, SEEK_END) == -1) {
        perror("lseek");
        return;
    }

    /*
     * Write one Student structure to the file.
     */
    ssize_t bytes = write(fd, &student, sizeof(Student));

    if (bytes != sizeof(Student)) {
        perror("write");
        return;
    }

    printf("Student added successfully.\n");
}

void list_students(int fd)
{
    Student student;

    /*
     * Move to the beginning of the file.
     */
    if (lseek(fd, 0, SEEK_SET) == -1) {
        perror("lseek");
        return;
    }

    printf("\n===== Student List =====\n");

    while (read(fd, &student, sizeof(Student)) == sizeof(Student)) {
        printf("ID   : %d\n", student.id);
        printf("Name : %s\n", student.name);
        printf("Age  : %d\n", student.age);
        printf("GPA  : %.2f\n", student.gpa);
        printf("------------------------\n");
    }
}

void find_student(int fd)
{
    Student student;
    int id;
    int found = 0;

    printf("Enter student ID: ");
    scanf("%d", &id);

    /*
     * Start scanning from the beginning.
     */
    if (lseek(fd, 0, SEEK_SET) == -1) {
        perror("lseek");
        return;
    }

    while (read(fd, &student, sizeof(Student)) == sizeof(Student)) {

        if (student.id == id) {
            printf("\n===== Student Found =====\n");
            printf("ID   : %d\n", student.id);
            printf("Name : %s\n", student.name);
            printf("Age  : %d\n", student.age);
            printf("GPA  : %.2f\n", student.gpa);

            found = 1;
            break;
        }
    }

    if (!found) {
        printf("Student with ID %d not found.\n", id);
    }
}

int main(void)
{
    int fd;
    int choice;

    /*
     * Open the binary file.
     *
     * O_RDWR : read and write
     * O_CREAT: create file if it does not exist
     */
    fd = open(FILE_NAME, O_RDWR | O_CREAT, 0644);

    if (fd == -1) {
        perror("open");
        return EXIT_FAILURE;
    }

    while (1) {

        printf("\n===== Student Management =====\n");
        printf("1. Add student\n");
        printf("2. List all students\n");
        printf("3. Find student by ID\n");
        printf("4. Exit\n");
        printf("Choose: ");

        scanf("%d", &choice);

        switch (choice) {

            case 1:
                add_student(fd);
                break;

            case 2:
                list_students(fd);
                break;

            case 3:
                find_student(fd);
                break;

            case 4:
                close(fd);
                printf("Program terminated.\n");
                return 0;

            default:
                printf("Invalid choice.\n");
        }
    }
}
