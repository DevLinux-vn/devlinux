#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
typedef struct
{
    int id;
    char name[64];
    int age;
    float gpa;
} Student;

void add_student(int fd);
void list_students(int fd);
void find_student(int fd);

void add_student(int fd)
{
    Student s;

    printf("ID: ");

    if (scanf("%d", &s.id) != 1)
    {
        printf("Invalid ID\n");
        return;
    }

    if (s.id <= 0)
    {
        printf("ID must be positive\n");
        return;
    }

    printf("Name: ");
    scanf(" %63[^\n]", s.name);

    printf("Age: ");

    if (scanf("%d", &s.age) != 1)
    {
        printf("Invalid age\n");
        return;
    }

    if (s.age <= 0 || s.age > 120)
    {
        printf("Invalid age\n");
        return;
    }

    printf("GPA: ");

    if (scanf("%f", &s.gpa) != 1)
    {
        printf("Invalid GPA\n");
        return;
    }

    if (s.gpa < 0.0f || s.gpa > 4.0f)
    {
        printf("Invalid GPA\n");
        return;
    }

    /*
     * Move file pointer to end of file
     * before appending new record.
     */
    if (lseek(fd, 0, SEEK_END) == (off_t)-1)
    {
        perror("lseek");
        return;
    }

    if (write(fd, &s, sizeof(Student))
        != sizeof(Student))
    {
        perror("write");
    }

    printf("Student added successfully\n");
}

void list_students(int fd)
{
    Student s;
    ssize_t bytes;

    if (lseek(fd, 0, SEEK_SET) == (off_t)-1)
    {
        perror("lseek");
        return;
    }

    while ((bytes =
            read(fd,
                 &s,
                 sizeof(Student)))
           == sizeof(Student))
    {
        printf("\n");
        printf("ID   : %d\n", s.id);
        printf("Name : %s\n", s.name);
        printf("Age  : %d\n", s.age);
        printf("GPA  : %.2f\n", s.gpa);
    }

    if (bytes < 0)
    {
        perror("read");
    }
}

void find_student(int fd)
{
    int id;
    int found = 0;

    Student s;

    printf("Enter ID: ");

    if (scanf("%d", &id) != 1)
    {
        printf("Invalid ID\n");
        return;
    }

    if (lseek(fd, 0, SEEK_SET) == (off_t)-1)
    {
        perror("lseek");
        return;
    }

    while (read(fd,
                &s,
                sizeof(Student))
           == sizeof(Student))
    {
        if (s.id == id)
        {
            printf("\n");
            printf("Student found\n");

            printf("ID   : %d\n", s.id);
            printf("Name : %s\n", s.name);
            printf("Age  : %d\n", s.age);
            printf("GPA  : %.2f\n", s.gpa);

            found = 1;
            break;
        }
    }

    if (!found)
    {
        printf("Student not found\n");
    }
}

int main(void)
{
    int fd;
    int choice;

    fd = open("students.dat",
              O_RDWR | O_CREAT,
              0644);

    if (fd < 0)
    {
        perror("open");
        return 1;
    }

    while (1)
    {
        printf("\n");
        printf("1. Add student\n");
        printf("2. List all students\n");
        printf("3. Find student by ID\n");
        printf("4. Exit\n");
        printf("Choice: ");

        if (scanf("%d", &choice) != 1)
        {
            printf("Invalid input\n");
            break;
        }

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
                return 0;

            default:
                printf("Invalid choice\n");
        }
    }

    close(fd);

    return 0;
}
