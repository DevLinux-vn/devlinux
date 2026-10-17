#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DATA_FILE "students.dat"

typedef struct {
    int id;
    char name[64];
    int age;
    float gpa;
} Student;

static void strip_newline(char *s)
{
    size_t len = strlen(s);

    if (len > 0 && s[len - 1] == '\n') {
        s[len - 1] = '\0';
    }
}

static int read_line(const char *prompt, char *buf, size_t size)
{
    printf("%s", prompt);
    fflush(stdout);

    if (fgets(buf, size, stdin) == NULL) {
        return 0;
    }

    strip_newline(buf);
    return 1;
}

static int read_int(const char *prompt, int *value)
{
    char buf[128];
    char extra;

    while (read_line(prompt, buf, sizeof(buf))) {
        if (sscanf(buf, "%d %c", value, &extra) == 1) {
            return 1;
        }
        printf("Invalid number. Please try again.\n");
    }

    return 0;
}

static int read_float(const char *prompt, float *value)
{
    char buf[128];
    char extra;

    while (read_line(prompt, buf, sizeof(buf))) {
        if (sscanf(buf, "%f %c", value, &extra) == 1) {
            return 1;
        }
        printf("Invalid number. Please try again.\n");
    }

    return 0;
}

static ssize_t write_full(int fd, const void *buf, size_t count)
{
    const char *p = buf;
    size_t written = 0;

    while (written < count) {
        ssize_t n = write(fd, p + written, count - written);
        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }
        written += (size_t)n;
    }

    return (ssize_t)written;
}

static void print_student(const Student *student)
{
    printf("ID: %d | Name: %s | Age: %d | GPA: %.2f\n",
           student->id,
           student->name,
           student->age,
           student->gpa);
}

static void add_student(int fd)
{
    Student student;

    memset(&student, 0, sizeof(student));

    if (!read_int("ID: ", &student.id) ||
        !read_line("Name: ", student.name, sizeof(student.name)) ||
        !read_int("Age: ", &student.age) ||
        !read_float("GPA: ", &student.gpa)) {
        printf("Input ended. Student was not added.\n");
        return;
    }

    if (lseek(fd, 0, SEEK_END) == (off_t)-1) {
        perror("lseek");
        return;
    }

    if (write_full(fd, &student, sizeof(student)) != (ssize_t)sizeof(student)) {
        perror("write");
        return;
    }

    printf("Student added successfully.\n");
}

static void list_students(int fd)
{
    Student student;
    int count = 0;

    if (lseek(fd, 0, SEEK_SET) == (off_t)-1) {
        perror("lseek");
        return;
    }

    printf("\n========== STUDENT LIST ==========\n");
    for (;;) {
        ssize_t n = read(fd, &student, sizeof(student));
        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }
            perror("read");
            break;
        }
        if (n == 0) {
            break;
        }
        if (n != (ssize_t)sizeof(student)) {
            printf("Warning: ignored a partial/corrupted record.\n");
            break;
        }

        printf("[%d] ", count);
        print_student(&student);
        count++;
    }

    if (count == 0) {
        printf("No students found.\n");
    }
    printf("==================================\n\n");
}

static void find_student(int fd)
{
    Student student;
    int target_id;

    if (!read_int("Enter ID to find: ", &target_id)) {
        printf("Input ended.\n");
        return;
    }

    if (lseek(fd, 0, SEEK_SET) == (off_t)-1) {
        perror("lseek");
        return;
    }

    for (;;) {
        ssize_t n = read(fd, &student, sizeof(student));
        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }
            perror("read");
            return;
        }
        if (n == 0) {
            break;
        }
        if (n != (ssize_t)sizeof(student)) {
            printf("Warning: ignored a partial/corrupted record.\n");
            break;
        }
        if (student.id == target_id) {
            printf("Found: ");
            print_student(&student);
            return;
        }
    }

    printf("Student with ID %d was not found.\n", target_id);
}

static void print_menu(void)
{
    printf("1. Add student\n");
    printf("2. List all students\n");
    printf("3. Find student by ID\n");
    printf("4. Exit\n");
    printf("Choose: ");
    fflush(stdout);
}

int main(void)
{
    int fd = open(DATA_FILE, O_RDWR | O_CREAT, 0644);
    char choice[32];

    if (fd < 0) {
        perror("open");
        return 1;
    }

    printf("Student manager using Linux system calls (%s)\n\n", DATA_FILE);

    for (;;) {
        print_menu();

        if (fgets(choice, sizeof(choice), stdin) == NULL) {
            break;
        }

        switch (atoi(choice)) {
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
            printf("Goodbye!\n");
            return 0;
        default:
            printf("Invalid choice.\n");
            break;
        }
    }

    close(fd);
    return 0;
}
