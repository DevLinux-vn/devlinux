#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_NAME "students.dat"
#define BUFFER_SIZE 256

typedef struct {
    int   id;
    char  name[64];
    int   age;
    float gpa;
} Student;


/*
 * Read one line from stdin using the read() system call.
 */
static void read_line(char *buffer, size_t size)
{
    size_t i = 0;
    char c;

    while (i < size - 1) {
        ssize_t n = read(STDIN_FILENO, &c, 1);

        if (n <= 0) {
            break;
        }

        if (c == '\n') {
            break;
        }

        buffer[i++] = c;
    }

    buffer[i] = '\0';
}


/*
 * Print a string using the write() system call.
 */
static void print_string(const char *str)
{
    write(STDOUT_FILENO, str, strlen(str));
}


/*
 * Read an integer from stdin.
 */
static int read_int(void)
{
    char buffer[BUFFER_SIZE];

    read_line(buffer, sizeof(buffer));

    return atoi(buffer);
}


/*
 * Read a float from stdin.
 */
static float read_float(void)
{
    char buffer[BUFFER_SIZE];

    read_line(buffer, sizeof(buffer));

    return strtof(buffer, NULL);
}


/*
 * Print one student's information.
 */
static void print_student(const Student *student)
{
    char buffer[BUFFER_SIZE];

    int len = snprintf(
        buffer,
        sizeof(buffer),
        "ID: %d\n"
        "Name: %s\n"
        "Age: %d\n"
        "GPA: %.2f\n"
        "--------------------\n",
        student->id,
        student->name,
        student->age,
        student->gpa
    );

    write(STDOUT_FILENO, buffer, len);
}


/*
 * Option 1: Add a student.
 */
static void add_student(int fd)
{
    Student student;

    memset(&student, 0, sizeof(Student));

    print_string("Enter student ID: ");
    student.id = read_int();

    print_string("Enter student name: ");
    read_line(student.name, sizeof(student.name));

    print_string("Enter student age: ");
    student.age = read_int();

    print_string("Enter student GPA: ");
    student.gpa = read_float();

    /*
     * Move file offset to the end so the new student
     * is appended to the existing records.
     */
    if (lseek(fd, 0, SEEK_END) == (off_t)-1) {
        print_string("Error: lseek failed.\n");
        return;
    }

    ssize_t written = write(fd, &student, sizeof(Student));

    if (written != sizeof(Student)) {
        print_string("Error: failed to write student.\n");
        return;
    }

    print_string("Student added successfully.\n");
}


/*
 * Option 2: List all students.
 */
static void list_students(int fd)
{
    Student student;
    ssize_t bytes_read;
    int count = 0;

    if (lseek(fd, 0, SEEK_SET) == (off_t)-1) {
        print_string("Error: lseek failed.\n");
        return;
    }

    while ((bytes_read = read(fd, &student, sizeof(Student))) > 0) {

        if (bytes_read != sizeof(Student)) {
            print_string("Error: incomplete student record.\n");
            return;
        }

        print_student(&student);
        count++;
    }

    if (count == 0) {
        print_string("\nNo students found.\n");
    }
}


/*
 * Option 3: Find a student by ID.
 */
static void find_student(int fd)
{
    Student student;
    int target_id;
    int found = 0;

    print_string("\nEnter student ID to find: ");
    target_id = read_int();

    if (lseek(fd, 0, SEEK_SET) == (off_t)-1) {
        print_string("Error: lseek failed.\n");
        return;
    }

    while (read(fd, &student, sizeof(Student)) == sizeof(Student)) {

        if (student.id == target_id) {
            print_string("Student found:\n");
            print_student(&student);

            found = 1;
            break;
        }
    }

    if (!found) {
        print_string("\nStudent not found.\n");
    }
}


int main(void)
{
    int fd;
    int choice;

    /*
     * Open existing file or create it if it does not exist.
     *
     * O_RDWR  : open for reading and writing
     * O_CREAT : create if the file does not exist
     */
    fd = open(FILE_NAME, O_RDWR | O_CREAT, 0644);

    if (fd == -1) {
        print_string("Error: cannot open students.dat\n");
        return 1;
    }

    while (1) {

        print_string(
            "\n"
            "===== Student Management =====\n"
            "1. Add student\n"
            "2. List all students\n"
            "3. Find student by ID\n"
            "4. Exit\n\n"
            "Choose an option: "
        );

        choice = read_int();

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
                print_string("Goodbye!\n\n");
                return 0;

            default:
                print_string("Invalid option. Please try again.\n");
                break;
        }
    }
}
