#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define FILE_NAME "students.dat"

typedef struct {
    int   id;
    char  name[64];
    int   age;
    float gpa;
} Student;

static void clear_input_buffer(void)
{
    int c;

    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

static int write_full(int fd, const void *buffer, size_t size)
{
    size_t total = 0;
    const char *buf = (const char *)buffer;

    while (total < size) {
        ssize_t n = write(fd, buf + total, size - total);

        if (n < 0) {
            if (errno == EINTR)
                continue;

            return -1;
        }

        total += (size_t)n;
    }

    return 0;
}

static void print_student(const Student *student)
{
    printf("ID   : %d\n", student->id);
    printf("Name : %s\n", student->name);
    printf("Age  : %d\n", student->age);
    printf("GPA  : %.2f\n", student->gpa);
    printf("-------------------------\n");
}

static void add_student(int fd)
{
    Student student;

    memset(&student, 0, sizeof(Student));

    printf("Enter ID: ");
    if (scanf("%d", &student.id) != 1) {
        printf("Invalid ID.\n");
        clear_input_buffer();
        return;
    }

    clear_input_buffer();

    printf("Enter name: ");
    if (fgets(student.name, sizeof(student.name), stdin) == NULL) {
        printf("Failed to read name.\n");
        return;
    }

    student.name[strcspn(student.name, "\n")] = '\0';

    printf("Enter age: ");
    if (scanf("%d", &student.age) != 1) {
        printf("Invalid age.\n");
        clear_input_buffer();
        return;
    }

    printf("Enter GPA: ");
    if (scanf("%f", &student.gpa) != 1) {
        printf("Invalid GPA.\n");
        clear_input_buffer();
        return;
    }

    clear_input_buffer();

    /*
     * Move file offset to the end of the file.
     * The new student will be appended here.
     */
    if (lseek(fd, 0, SEEK_END) == (off_t)-1) {
        perror("lseek");
        return;
    }

    if (write_full(fd, &student, sizeof(Student)) < 0) {
        perror("write");
        return;
    }

    printf("Student added successfully.\n");
}

static void list_students(int fd)
{
    Student student;
    ssize_t bytes_read;
    int count = 0;

    /*
     * Start reading from the beginning of the file.
     */
    if (lseek(fd, 0, SEEK_SET) == (off_t)-1) {
        perror("lseek");
        return;
    }

    while (1) {
        bytes_read = read(fd, &student, sizeof(Student));

        if (bytes_read < 0) {
            if (errno == EINTR)
                continue;

            perror("read");
            return;
        }

        if (bytes_read == 0)
            break;

        if ((size_t)bytes_read != sizeof(Student)) {
            const char msg[] = "Error: incomplete student record.\n";
            write(STDERR_FILENO, msg, sizeof(msg) - 1);
            return;
        }

        print_student(&student);
        count++;
    }

    if (count == 0) {
        printf("No students found.\n");
    } else {
        printf("Total students: %d\n", count);
    }
}

static void find_student(int fd)
{
    Student student;
    ssize_t bytes_read;
    int search_id;
    int found = 0;

    printf("Enter student ID: ");

    if (scanf("%d", &search_id) != 1) {
        printf("Invalid ID.\n");
        clear_input_buffer();
        return;
    }

    clear_input_buffer();

    /*
     * Start scanning from the beginning.
     */
    if (lseek(fd, 0, SEEK_SET) == (off_t)-1) {
        perror("lseek");
        return;
    }

    while (1) {
        bytes_read = read(fd, &student, sizeof(Student));

        if (bytes_read < 0) {
            if (errno == EINTR)
                continue;

            perror("read");
            return;
        }

        if (bytes_read == 0)
            break;

        if ((size_t)bytes_read != sizeof(Student)) {
            const char msg[] = "Error: incomplete student record.\n";
            write(STDERR_FILENO, msg, sizeof(msg) - 1);
            return;
        }

        if (student.id == search_id) {
            printf("Student found:\n");
            print_student(&student);
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("Student with ID %d not found.\n", search_id);
    }
}

static void print_menu(void)
{
    printf("\n");
    printf("===== Student Management =====\n");
    printf("1. Add student\n");
    printf("2. List all students\n");
    printf("3. Find student by ID\n");
    printf("4. Exit\n");
    printf("Choose an option: ");
}

int main(void)
{
    int fd;
    int choice;

    /*
     * Open students.dat for reading and writing.
     * Create it if it does not exist.
     */
    fd = open(FILE_NAME, O_RDWR | O_CREAT, 0644);

    if (fd < 0) {
        perror("open");
        return EXIT_FAILURE;
    }

    while (1) {
        print_menu();

        if (scanf("%d", &choice) != 1) {
            printf("Invalid choice.\n");
            clear_input_buffer();
            continue;
        }

        clear_input_buffer();

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
            if (close(fd) < 0) {
                perror("close");
                return EXIT_FAILURE;
            }

            printf("Goodbye!\n");
            return EXIT_SUCCESS;

        default:
            printf("Invalid choice. Please select 1-4.\n");
            break;
        }
    }

    return EXIT_SUCCESS;
}
