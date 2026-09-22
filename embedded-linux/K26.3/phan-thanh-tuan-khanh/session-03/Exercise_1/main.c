#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define FILE_NAME "students.dat"
#define NAME_SIZE 64

typedef struct
{
    int id;
    char name[NAME_SIZE];
    int age;
    float gpa;
} Student;

static void clearInputBuffer(void)
{
    int ch;

    while ((ch = getchar()) != '\n' &&
           ch != EOF)
    {
    }
}

static int writeStudent(int fd, const Student *s)
{
    const char *ptr = (const char *)s;
    size_t total = 0;

    while (total < sizeof(Student))
    {
        ssize_t n = write(fd,
                          ptr + total,
                          sizeof(Student) - total);

        if (n < 0)
        {
            perror("write");
            return -1;
        }

        total += n;
    }

    return 0;
}

void addStudent(int fd)
{
    Student s;

    printf("Enter ID: ");

    if (scanf("%d", &s.id) != 1)
    {
        fprintf(stderr, "Invalid ID\n");
        clearInputBuffer();
        return;
    }

    clearInputBuffer();

    printf("Enter name: ");

    if (scanf("%63[^\n]", s.name) != 1)
    {
        fprintf(stderr, "Invalid name\n");
        clearInputBuffer();
        return;
    }

    clearInputBuffer();

    printf("Enter age: ");

    if (scanf("%d", &s.age) != 1)
    {
        fprintf(stderr, "Invalid age\n");
        clearInputBuffer();
        return;
    }

    clearInputBuffer();

    printf("Enter GPA: ");

    if (scanf("%f", &s.gpa) != 1)
    {
        fprintf(stderr, "Invalid GPA\n");
        clearInputBuffer();
        return;
    }

    clearInputBuffer();

    if (lseek(fd, 0, SEEK_END) == (off_t)-1)
    {
        perror("lseek");
        return;
    }

    if (writeStudent(fd, &s) == -1)
    {
        return;
    }

    printf("Student added successfully.\n");
}

void listStudents(int fd)
{
    Student s;

    if (lseek(fd, 0, SEEK_SET) == (off_t)-1)
    {
        perror("lseek");
        return;
    }

    printf("\n===== Student List =====\n");

    while (1)
    {
        ssize_t n = read(fd,
                         &s,
                         sizeof(Student));

        if (n == -1)
        {
            perror("read");
            break;
        }

        if (n == 0)
        {
            /* EOF */
            break;
        }

        if (n != sizeof(Student))
        {
            fprintf(stderr,
                    "Warning: partial record detected\n");
            break;
        }

        printf("ID: %d\n", s.id);
        printf("Name: %s\n", s.name);
        printf("Age: %d\n", s.age);
        printf("GPA: %.2f\n", s.gpa);
        printf("------------------------\n");
    }
}

void findStudent(int fd)
{
    int targetId;
    Student s;
    int found = 0;

    printf("Enter ID to find: ");

    if (scanf("%d", &targetId) != 1)
    {
        fprintf(stderr, "Invalid ID\n");
        clearInputBuffer();
        return;
    }

    clearInputBuffer();

    if (lseek(fd, 0, SEEK_SET) == (off_t)-1)
    {
        perror("lseek");
        return;
    }

    while (1)
    {
        ssize_t n = read(fd,
                         &s,
                         sizeof(Student));

        if (n == -1)
        {
            perror("read");
            return;
        }

        if (n == 0)
        {
            break;
        }

        if (n != sizeof(Student))
        {
            fprintf(stderr,
                    "Warning: partial record detected\n");
            break;
        }

        if (s.id == targetId)
        {
            printf("\nStudent Found:\n");
            printf("ID: %d\n", s.id);
            printf("Name: %s\n", s.name);
            printf("Age: %d\n", s.age);
            printf("GPA: %.2f\n", s.gpa);

            found = 1;
            break;
        }
    }

    if (!found)
    {
        printf("Student with ID %d not found.\n",
               targetId);
    }
}

void showMenu(int fd)
{
    int choice;

    while (1)
    {
        printf("\n===== MENU =====\n");
        printf("1. Add student\n");
        printf("2. List all students\n");
        printf("3. Find student by ID\n");
        printf("4. Exit\n");
        printf("Choose: ");

        if (scanf("%d", &choice) != 1)
        {
            fprintf(stderr, "Invalid choice\n");
            clearInputBuffer();
            continue;
        }

        clearInputBuffer();

        switch (choice)
        {
            case 1:
                addStudent(fd);
                break;

            case 2:
                listStudents(fd);
                break;

            case 3:
                findStudent(fd);
                break;

            case 4:
                return;

            default:
                printf("Invalid choice.\n");
        }
    }
}

int main(void)
{
    int fd = open(FILE_NAME,
                  O_RDWR | O_CREAT,
                  0644);

    if (fd < 0)
    {
        perror("open");
        return 1;
    }

    showMenu(fd);

    if (close(fd) == -1)
    {
        perror("close");
        return 1;
    }

    printf("Bye!\n");

    return 0;
}