#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define FILE_NAME "students.dat"

typedef struct {
    int id;
    char name[64];
    int age;
    float gpa;
} Student;

void add_student(int fd)
{
    Student st;

    printf("Enter ID: ");
    scanf("%d", &st.id);

    printf("Enter Name: ");
    scanf(" %63[^\n]", st.name);

    printf("Enter Age: ");
    scanf("%d", &st.age);

    printf("Enter GPA: ");
    scanf("%f", &st.gpa);

    lseek(fd, 0, SEEK_END);

    if (write(fd, &st, sizeof(Student)) != sizeof(Student))
    {
        perror("write");
        return;
    }

    printf("Student added successfully.\n");
}

void list_students(int fd)
{
    Student st;

    lseek(fd, 0, SEEK_SET);

    printf("\n===== STUDENT LIST =====\n");

    while (read(fd, &st, sizeof(Student)) == sizeof(Student))
    {
        printf("ID   : %d\n", st.id);
        printf("Name : %s\n", st.name);
        printf("Age  : %d\n", st.age);
        printf("GPA  : %.2f\n", st.gpa);
        printf("------------------------\n");
    }
}

void find_student(int fd)
{
    int target_id;
    Student st;
    int found = 0;

    printf("Enter ID to find: ");
    scanf("%d", &target_id);

    lseek(fd, 0, SEEK_SET);

    while (read(fd, &st, sizeof(Student)) == sizeof(Student))
    {
        if (st.id == target_id)
        {
            printf("\nStudent found:\n");
            printf("ID   : %d\n", st.id);
            printf("Name : %s\n", st.name);
            printf("Age  : %d\n", st.age);
            printf("GPA  : %.2f\n", st.gpa);

            found = 1;
            break;
        }
    }

    if (!found)
    {
        printf("Student with ID %d not found.\n", target_id);
    }
}

int main(void)
{
    int choice;

    int fd = open(FILE_NAME, O_RDWR | O_CREAT, 0644);
    if (fd < 0)
    {
        perror("open");
        return 1;
    }

    while (1)
    {
        printf("\n===== MENU =====\n");
        printf("1. Add student\n");
        printf("2. List all students\n");
        printf("3. Find student by ID\n");
        printf("4. Exit\n");
        printf("Choose: ");

        scanf("%d", &choice);

        switch (choice)
        {
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
                printf("Goodbye!\n");
                return 0;

            default:
                printf("Invalid choice.\n");
        }
    }

    return 0;
}