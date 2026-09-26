#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

typedef struct
{
    int id;
    char name[64];
    int age;
    float gpa;
} Student;

// Helper to safely double buffer capacity when full
static Student *ensure_capacity(Student *array, int count, int *capacity)
{
    if (count < *capacity)
    {
        return array;
    }
    int new_cap = (*capacity == 0) ? 4 : (*capacity * 2);
    Student *temp = realloc(array, sizeof(Student) * new_cap);
    if (!temp)
    {
        perror("realloc failed");
        return NULL;
    }
    *capacity = new_cap;
    return temp;
}

// Reads all students from students.bin into dynamic array
Student *readAllStudent(int *out_count, int *out_cap)
{
    printf("Retrieving existing records...\n");

    int count = 0;
    int capacity = 4;
    Student *students = malloc(sizeof(Student) * capacity);
    if (!students)
    {
        perror("malloc failed");
        return NULL;
    }

    int fd = open("students.bin", O_RDONLY);
    if (fd == -1)
    {
        if (errno == ENOENT)
        {
            // File does not exist yet; return empty array
            *out_count = 0;
            *out_cap = capacity;
            return students;
        }
        perror("open failed");
        free(students);
        return NULL;
    }

    Student s;
    while (read(fd, &s, sizeof(Student)) == sizeof(Student))
    {
        Student *temp = ensure_capacity(students, count, &capacity);
        if (!temp)
        {
            free(students);
            close(fd);
            return NULL;
        }
        students = temp;
        students[count++] = s;
    }

    close(fd);
    *out_count = count;
    *out_cap = capacity;
    return students;
}

int main(void)
{
    int count = 0;
    int capacity = 0;

    Student *students = readAllStudent(&count, &capacity);
    if (!students)
    {
        return EXIT_FAILURE;
    }

    int fd = open("students.bin", O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd == -1)
    {
        perror("Failed to open students.bin for writing");
        free(students);
        return EXIT_FAILURE;
    }

    int isExit = 0;
    while (!isExit)
    {
        printf("\n--- Menu ---\n");
        printf("1: Add\n2: List\n3: Find\n4: Exit\nCommand: ");

        int command;
        if (scanf("%d", &command) != 1)
        {
            fprintf(stderr, "Invalid input. Exiting...\n");
            break;
        }

        switch (command)
        {
        case 1:
        {
            Student s = {.id = count};

            Student *temp = ensure_capacity(students, count, &capacity);
            if (!temp)
            {
                isExit = 1;
                break;
            }
            students = temp;

            printf("Enter Name: ");
            if (scanf(" %63[^\n]", s.name) != 1)
            {
                fprintf(stderr, "Error reading name.\n");
                break;
            }

            printf("Enter Age: ");
            if (scanf("%d", &s.age) != 1)
            {
                fprintf(stderr, "Error reading age.\n");
                break;
            }

            printf("Enter GPA: ");
            if (scanf("%f", &s.gpa) != 1)
            {
                fprintf(stderr, "Error reading GPA.\n");
                break;
            }

            if (write(fd, &s, sizeof(Student)) != sizeof(Student))
            {
                perror("write failed");
                break;
            }

            students[count++] = s;
            printf("Student added successfully (ID: %d).\n", s.id);
            break;
        }
        case 2:
            if (count == 0)
            {
                printf("No students found.\n");
            }
            else
            {
                for (int i = 0; i < count; i++)
                {
                    printf("ID: %d | Name: %-15s | Age: %d | GPA: %.2f\n",
                           students[i].id, students[i].name, students[i].age, students[i].gpa);
                }
            }
            break;

        case 3:
        {
            int search_id = -1;
            int found = 0;
            printf("Enter ID: ");
            if (scanf("%d", &search_id) != 1)
            {
                fprintf(stderr, "Invalid ID input.\n");
                break;
            }
            for (int i = 0; i < count; i++)
            {
                if (students[i].id == search_id)
                {
                    found = 1;
                    printf("ID: %d | Name: %s | Age: %d | GPA: %.2f\n",
                           students[i].id, students[i].name, students[i].age, students[i].gpa);
                    break;
                }
            }
            if (!found)
            {
                printf("Student with ID %d not found.\n", search_id);
            }
            break;
        }
        case 4:
            isExit = 1;
            break;

        default:
            printf("Command not found. Please choose 1-4.\n");
            break;
        }
    }

    // Single point of cleanup
    close(fd);
    free(students);
    return EXIT_SUCCESS;
}