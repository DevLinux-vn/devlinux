#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
typedef struct {
    int   id;
    char  name[64];
    int   age;
    float gpa;
} Student;


/* In thông tin 1 sinh viên */
void print_student(const Student *s)
{
    printf("ID   : %d\n", s->id);
    printf("Name : %s\n", s->name);
    printf("Age  : %d\n", s->age);
    printf("GPA  : %.2f\n", s->gpa);
}


/* Thêm 1 sinh viên vào cuối file */
void add_student(int fd)
{
    Student s;

    printf("ID: ");
    scanf("%d", &s.id);

    printf("Name: ");
    scanf(" %63[^\n]", s.name);

    printf("Age: ");
    scanf("%d", &s.age);

    printf("GPA: ");
    scanf("%f", &s.gpa);

    /* Di chuyển vị trí ghi xuống cuối file */
    lseek(fd, 0, SEEK_END);

    /* Ghi nguyên struct Student xuống file */
    if (write(fd, &s, sizeof(Student)) != sizeof(Student)) {
        perror("write");
        return;
    }

    printf("Student added.\n");
}


/* In toàn bộ sinh viên */
void list_students(int fd)
{
    Student s;

    /* Quay về đầu file */
    lseek(fd, 0, SEEK_SET);

    while (read(fd, &s, sizeof(Student)) == sizeof(Student)) {
        print_student(&s);
        printf("-----------------\n");
    }
}


/* Tìm sinh viên theo ID */
void find_student(int fd)
{
    Student s;
    int target_id;

    printf("Enter ID: ");
    scanf("%d", &target_id);

    /* Quay về đầu file */
    lseek(fd, 0, SEEK_SET);

    while (read(fd, &s, sizeof(Student)) == sizeof(Student)) {

        if (s.id == target_id) {
            print_student(&s);
            return;
        }
    }

    printf("Student not found.\n");
}


int main(void)
{
    int fd;
    int choice;

    fd = open("students.dat", O_RDWR | O_CREAT, 0644);

    if (fd == -1) {
        perror("open");
        return 1;
    }

    while (1) {
        printf("\n===== STUDENT MENU =====\n");
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
                printf("Exit program.\n");
                return 0;

            default:
                printf("Invalid choice!\n");
        }
    }
}