#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define FILE_NAME "students.dat"

typedef struct {
    int   id;
    char  name[64];
    int   age;
    float gpa;
} Student;

static void add_student(void) {
    int fd = open(FILE_NAME, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd < 0) { perror("open"); return; }

    Student s;
    printf("ID: ");    scanf("%d", &s.id);
    printf("Name: ");  scanf(" %63[^\n]", s.name);
    printf("Age: ");   scanf("%d", &s.age);
    printf("GPA: ");   scanf("%f", &s.gpa);

    if (write(fd, &s, sizeof(s)) != sizeof(s))
        perror("write");
    close(fd);
}

static void list_students(void) {
    int fd = open(FILE_NAME, O_RDONLY);
    if (fd < 0) { perror("open"); return; }

    Student s;
    ssize_t n;
    int count = 0;
    while ((n = read(fd, &s, sizeof(s))) == sizeof(s)) {
        printf("[%d] ID=%d  Name=%s  Age=%d  GPA=%.2f\n",
               ++count, s.id, s.name, s.age, s.gpa);
    }
    if (n < 0) perror("read");
    if (count == 0) printf("(no records)\n");
    close(fd);
}

static void find_student(void) {
    int target;
    printf("ID to find: ");
    scanf("%d", &target);

    int fd = open(FILE_NAME, O_RDONLY);
    if (fd < 0) { perror("open"); return; }

    Student s;
    while (read(fd, &s, sizeof(s)) == sizeof(s)) {
        if (s.id == target) {
            printf("FOUND: ID=%d Name=%s Age=%d GPA=%.2f\n",
                   s.id, s.name, s.age, s.gpa);
            close(fd);
            return;
        }
    }
    printf("Student id=%d not found.\n", target);
    close(fd);
}

int main(void) {
    int choice;
    while (1) {
        printf("\n1. Add  2. List  3. Find  4. Exit\nChoice: ");
        if (scanf("%d", &choice) != 1) break;

        switch (choice) {
            case 1: add_student();   break;
            case 2: list_students(); break;
            case 3: find_student();  break;
            case 4: return 0;
            default: printf("Invalid.\n");
        }
    }
    return 0;
}
