#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct {
    int   id;
    char  name[64];
    int   age;
    float gpa;
} Student;

void Display_Menu(void)
{
    printf("1. Add student\n");
    printf("2. List all students\n");
    printf("3. Find student by ID\n");
    printf("4. Exit\n");
}

void Add_Student(void)
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

    /* Open file for writing */
    int fd = open("student.dat",  O_CREAT | O_WRONLY | O_APPEND, 0666);

    if (fd == -1) {
        printf("[ERROR] Cannot open file\n");
        return;
    }

    /* Write struct as raw bytes */
    ssize_t bytes = write(fd, &student, sizeof(Student));

    if (bytes == -1) {
        printf("[ERROR] Cannot write file\n");
        close(fd);
        return;
    }
    close(fd);
}

void Find_Student()
{
    Student student;
    int id;

    printf("Enter student's id: ");
    scanf(" %d", &id);

    /* Open file for reading */
    int fd = open("student.dat", O_RDONLY);

    if (fd == -1) {
        printf("[ERROR] Cannot open file\n");
        return;
    }

    while(read(fd, &student, sizeof(Student)) > 0)
    {
        if(student.id == id)
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
    printf("Student with id %d is not exist\n", id);
    close(fd);
}

void List_Students(void)
{
    Student student;

    /* Open file for reading */
    int fd = open("student.dat", O_RDONLY);

    if (fd == -1) {
        printf("[ERROR] Cannot open file\n");
        return;
    }

    while(read(fd, &student, sizeof(Student)) > 0)
    {
        printf("=================================\n");
        printf("Student's id: %d\n", student.id);
        printf("Student's name: %s\n", student.name);
        printf("Student's age: %d\n", student.age);
        printf("Student's gpa: %0.2f\n", student.gpa);
        printf("=================================\n");
    }
    close(fd);
}

int main()
{
    char KeyInput;
    while(1)
    {
        Display_Menu();
        scanf(" %c", &KeyInput);
        switch(KeyInput)
        {
            case '1':
                Add_Student();
                break;
            case '2':
                List_Students();
                break;
            case '3':
                Find_Student();
                break;
            case '4':
                return 0;
            default:
                break;
        }
    }
    return 0;
}