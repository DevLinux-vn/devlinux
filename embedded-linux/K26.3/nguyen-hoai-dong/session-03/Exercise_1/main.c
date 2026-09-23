#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define NAME_SIZE 64

typedef struct {
    int   id;
    char  name[NAME_SIZE];
    int   age;
    float gpa;
}Student;

void display_menu(void)
{
    printf("1. Add student\n");
    printf("2. List all students\n");
    printf("3. Find student by ID\n");
    printf("4. Exit\n");
}

void add_student(void)
{
    Student student;
    printf("Enter student's id: ");
    scanf(" %d", &student.id);
    printf("Enter student's name: ");
    scanf("%63s", student.name);
    printf("Enter student's age: ");
    scanf("%d", &student.age);
    printf("Enter student's gpa: ");
    scanf("%f", &student.gpa);

    int fd = open("students.dat",  O_CREAT | O_WRONLY | O_APPEND, 0666);
    if (fd == -1) 
    {
        printf("[ERROR] Cannot open file\n");
        return;
    }

    ssize_t bytes = write(fd, &student, sizeof(Student));
    if (bytes == -1) 
    {
        printf("[ERROR] Cannot write file\n");
    } 
    else if (bytes != sizeof(Student))
    {
        printf("[ERROR] Writting file incorrectly\n");
    }
    else
    {
        printf("[INF] Writting file successfully\n");
    }
    close(fd);
}

void find_student()
{
    Student student;
    int id;

    printf("Enter student's id: ");
    scanf(" %d", &id);

    int fd = open("students.dat", O_RDONLY);
    if (fd == -1) {
        printf("[ERROR] Cannot open file\n");
        return;
    }

    while(1)
    {
        ssize_t bytes = read(fd, &student, sizeof(Student));

        if (bytes < 0 )
        {
            if (errno == EINTR)
            {
                continue;
            }

            printf("[ERROR] Cannot read file\n");
            close(fd);
            return;
        }
        else if (bytes == 0)
        {
            printf("Student with id %d is not exist\n", id);
            close(fd);
            return;
        }
        else
        {
            if (student.id == id)
            {
                printf("=================================\n");
                printf("Has founded\n");
                printf("Student's id: %d\n", student.id);
                printf("Student's name: %s\n", student.name);
                printf("Student's age: %d\n", student.age);
                printf("Student's gpa: %0.2f\n", student.gpa);
                printf("=================================\n");
                close(fd);
                return;
            }
        }
    }
}

void list_students(void)
{
    Student student;

    int fd = open("students.dat", O_RDONLY);
    if (fd == -1) {
        printf("[ERROR] Cannot open file\n");
        return;
    }

    if (read(fd, &student, sizeof(Student)) == 0)
    {
        printf("[INF] Empty file\n");
        close(fd);
        return;
    }
    
    lseek(fd, 0, SEEK_SET);
    while(1)
    {
        ssize_t bytes = read(fd, &student, sizeof(Student));

        if (bytes < 0 )
        {
            if (errno == EINTR)
            {
                continue;
            }

            printf("[ERROR] Cannot read file\n");
            close(fd);
            return;
        }
        else if (bytes == 0)
        {
            close(fd);
            return;
        }
        else
        {
            printf("=================================\n");
            printf("Student's id: %d\n", student.id);
            printf("Student's name: %s\n", student.name);
            printf("Student's age: %d\n", student.age);
            printf("Student's gpa: %0.2f\n", student.gpa);
            printf("=================================\n");
        }
    }
}

int main()
{
    char key_input;
    while(1)
    {
        display_menu();
        scanf(" %c", &key_input);
        switch(key_input)
        {
            case '1':
                add_student();
                break;
            case '2':
                list_students();
                break;
            case '3':
                find_student();
                break;
            case '4':
                return 0;
            default:
                break;
        }
    }
    return 0;
}