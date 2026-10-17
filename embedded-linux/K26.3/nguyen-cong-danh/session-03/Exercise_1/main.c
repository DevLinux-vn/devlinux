#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define FILE_NAME "students.dat"

typedef struct
{
    int id;
    char name[64];
    int age;
    float gpa;
} Student;

void add_student(int fd)
{
    Student st;

    printf("Enter ID: ");
    if (scanf("%d", &st.id) != 1)
    {
        fprintf(stderr, "Invalid ID input\n");
        return;
    }

    printf("Enter Name: ");
    if (scanf(" %63[^\n]", st.name) != 1)
    {
        fprintf(stderr, "Invalid Name input\n");
        return;
    }

    printf("Enter Age: ");
    if (scanf("%d", &st.age) != 1)
    {
        fprintf(stderr, "Invalid Age input\n");
        return;
    }

    printf("Enter GPA: ");
    if (scanf("%f", &st.gpa) != 1)
    {
        fprintf(stderr, "Invalid GPA input\n");
        return;
    }

    if (lseek(fd, 0, SEEK_END) == -1)
    {
        perror("lseek");
        return;
    }

    ssize_t written = 0;
    ssize_t total_size = sizeof(Student);
    char *ptr = (char *)&st;

    while (written < total_size)
    {
        ssize_t n = write(fd,
                          ptr + written,
                          total_size - written);

        if (n < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }

            perror("write");
            return;
        }

        written += n;
    }

    printf("Student added successfully.\n");
}

void list_students(int fd)
{
    Student st;

    if (lseek(fd, 0, SEEK_SET) == -1)
    {
        perror("lseek");
        return;
    }

    printf("\n===== STUDENT LIST =====\n");

    while (1)
    {
        ssize_t n = read(fd, &st, sizeof(Student));

        if (n < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }

            perror("read");
            return;
        }

        if (n == 0)
        {
            break;
        }

        if (n != (ssize_t)sizeof(Student))
        {
            fprintf(stderr, "Partial record detected.\n");
            break;
        }

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
    int found = 0;
    Student st;

    printf("Enter ID to find: ");

    if (scanf("%d", &target_id) != 1)
    {
        fprintf(stderr, "Invalid ID input\n");
        return;
    }

    if (lseek(fd, 0, SEEK_SET) == -1)
    {
        perror("lseek");
        return;
    }

    while (1)
    {
        ssize_t n = read(fd, &st, sizeof(Student));

        if (n < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }

            perror("read");
            return;
        }

        if (n == 0)
        {
            break;
        }

        if (n != (ssize_t)sizeof(Student))
        {
            fprintf(stderr, "Partial record detected.\n");
            return;
        }

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

        if (scanf("%d", &choice) != 1)
        {
            fprintf(stderr, "Invalid menu input\n");

            if (close(fd) == -1)
            {
                perror("close");
            }

            return 1;
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
                if (close(fd) == -1)
                {
                    perror("close");
                    return 1;
                }

                printf("Goodbye!\n");
                return 0;

            default:
                printf("Invalid choice.\n");
                break;
        }
    }

    return 0;
}