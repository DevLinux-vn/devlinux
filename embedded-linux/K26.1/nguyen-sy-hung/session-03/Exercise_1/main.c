#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define FILE_NAME "students.dat"


typedef struct {
    int id;
    char name[64];
    int age;
    float gpa;
} Student;


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


    lseek(fd, 0, SEEK_END);

    write(fd, &student, sizeof(Student));
}


void list_students(int fd)
{
    Student student;

    lseek(fd, 0, SEEK_SET);

    while (read(fd, &student, sizeof(Student)) == sizeof(Student))
    {
        printf("ID: %d\n", student.id);
        printf("Name: %s\n", student.name);
        printf("Age: %d\n", student.age);
        printf("GPA: %.2f\n", student.gpa);
        printf("------------------\n");
    }
}


void find_student(int fd)
{
    int id;
    Student student;
    int found = 0;


    printf("Enter ID to find: ");
    scanf("%d", &id);


    lseek(fd, 0, SEEK_SET);


    while (read(fd, &student, sizeof(Student)) == sizeof(Student))
    {
        if (student.id == id)
        {
            printf("ID: %d\n", student.id);
            printf("Name: %s\n", student.name);
            printf("Age: %d\n", student.age);
            printf("GPA: %.2f\n", student.gpa);

            found = 1;
            break;
        }
    }


    if (!found)
    {
        printf("Student not found\n");
    }
}


int main()
{
    int fd;
    int choice;


    fd = open(FILE_NAME, O_RDWR | O_CREAT, 0644);

    if (fd < 0)
    {
        printf("Cannot open file\n");
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
                return 0;

            default:
                printf("Invalid choice\n");
        }
    }
}