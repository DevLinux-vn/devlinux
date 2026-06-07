#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

typedef struct {
    int   id;
    char  name[64];
    int   age;
    float gpa;
} Student;

static void print_student(const Student *s)
{
    printf("  ID   : %d\n",   s->id);
    printf("  Name : %s\n",   s->name);
    printf("  Age  : %d\n",   s->age);
    printf("  GPA  : %.2f\n", s->gpa);
}

static void add_student(int fd)
{
    Student s;
    memset(&s, 0, sizeof(s));

    printf("Enter ID   : "); scanf("%d",  &s.id);
    printf("Enter Name : "); scanf(" %63[^\n]", s.name);
    printf("Enter Age  : "); scanf("%d",  &s.age);
    printf("Enter GPA  : "); scanf("%f",  &s.gpa);

    lseek(fd, 0, SEEK_END);
    write(fd, &s, sizeof(Student));
    printf("Student added.\n");
}

static void list_students(int fd)
{
    Student s;
    int count = 0;

    lseek(fd, 0, SEEK_SET);
    while (read(fd, &s, sizeof(Student)) == sizeof(Student)) {
        printf("--- Record %d ---\n", ++count);
        print_student(&s);
    }

    if (count == 0)
        printf("No records found.\n");
}

static void find_student(int fd)
{
    int target;
    printf("Enter ID to find: "); scanf("%d", &target);

    Student s;
    lseek(fd, 0, SEEK_SET);
    while (read(fd, &s, sizeof(Student)) == sizeof(Student)) {
        if (s.id == target) {
            printf("Found:\n");
            print_student(&s);
            return;
        }
    }
    printf("Student with ID %d not found.\n", target);
}

int main(void)
{
    int fd = open("students.dat", O_RDWR | O_CREAT, 0644);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    int choice;
    do {
        printf("\n=== Student Manager ===\n");
        printf("1. Add student\n");
        printf("2. List all students\n");
        printf("3. Find student by ID\n");
        printf("4. Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);

        switch (choice) {
        case 1: add_student(fd);  break;
        case 2: list_students(fd); break;
        case 3: find_student(fd);  break;
        case 4: printf("Goodbye.\n"); break;
        default: printf("Invalid option.\n"); break;
        }
    } while (choice != 4);

    close(fd);
    return 0;
}
