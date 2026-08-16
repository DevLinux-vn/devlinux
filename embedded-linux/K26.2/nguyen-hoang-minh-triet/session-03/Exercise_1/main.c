#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

typedef struct {
    int id;
    char name[64];
    int age;
    float gpa;
} Student_t;


#define FILE_NAME "students.dat"

/*
 * @brief: This function writes a student record to a binary file.
 * @param: student - pointer to the student record to be written.
 * @return: void
 */
static void print_student(const Student_t *student);

/*
 * @brief: This function adds a student record to the binary file.
 * @param: fd - file descriptor of the binary file.
 * @return: 0 on success, -1 on failure.
 */
static int add_student(int fd);

/*
 * @brief: This function lists all student records in the binary file.
 * @param: fd - file descriptor of the binary file.
 * @return: 0 on success, -1 on failure.
 */
static int list_students(int fd);

/*
 * @brief: This function finds a student record by ID in the binary file.
 * @param: fd - file descriptor of the binary file.
 * @return: 0 on success, -1 on failure.
 */
static int find_student(int fd);

int main(void)  {
    
    int fd;
    int choice;

    /*
     * Open the binary file.
     *
     * O_RDWR:
     *     read + write
     *
     * O_CREAT:
     *     create file if it does not exist
     *
     * 0644:
     *     owner: read/write
     *     others: read
     */

    fd = open(FILE_NAME, O_RDWR | O_CREAT, 0644);

    if (fd == -1) {
        perror("open");
        return EXIT_FAILURE;
    }

    while (1) {

        printf("\n");
        printf("===== Student Management =====\n");
        printf("1. Add student\n");
        printf("2. List all students\n");
        printf("3. Find student by ID\n");
        printf("4. Exit\n");
        printf("Choose: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid choice.\n");

            /*
             * Clear invalid input from stdin.
             */
            while (getchar() != '\n') {
                /* discard */
            }

            continue;
        }

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
                if (close(fd) == -1) {
                    perror("close");
                    return EXIT_FAILURE;
                }

                printf("Goodbye.\n");
                return EXIT_SUCCESS;

            default:
                printf("Invalid choice.\n");
                break;
        }
    }
}

static void print_student(const Student_t *student) {
    printf("ID  : %d\n", student->id);
    printf("Name: %s\n", student->name);
    printf("Age : %d\n", student->age);
    printf("GPA : %.2f\n", student->gpa);
}

static int add_student(int fd) {

    Student_t student;
    ssize_t bytes_written;

    printf("Enter ID: ");
    if (scanf("%d", &student.id) != 1)  {
        printf("Invalid ID.\n");
        return -1;
    }

    printf("Enter name: ");
    if (scanf(" %63[^\n]", student.name) != 1)  {
        printf("Invalid name.\n");
        return -1;
    }

    printf("Enter age: ");
    if (scanf("%d", &student.age) != 1)  {
        printf("Invalid age.\n");
        return -1;
    }

    printf("Enter GPA: ");
    if (scanf("%f", &student.gpa) != 1)  {
        printf("Invalid GPA.\n");
        return -1;
    }

    /*
     * Move to the end of the file before appending.
     */
    if (lseek(fd, 0, SEEK_END) == (off_t)-1)    {
        perror("lseek");
        return -1;
    }

    bytes_written = write(fd, &student, sizeof(Student_t));

    if (bytes_written != (ssize_t)sizeof(Student_t))    {
        perror("write");
        return -1;
    }

    printf("Student added successfully.\n");

    return 0;
}

static int list_students(int fd) {
    Student_t student;
    ssize_t bytes_read;
    int count = 0;

    /*
     * Start reading from the beginning.
     */
    if (lseek(fd, 0, SEEK_SET) == (off_t)-1)    {
        perror("lseek");
        return -1;
    }

    while (1) {

        bytes_read = read(fd, &student, sizeof(Student_t));

        if (bytes_read == 0) {
            break;
        }

        if (bytes_read != (ssize_t)sizeof(Student_t)) {
            printf("Error: corrupted or incomplete record.\n");
            return -1;
        }

        printf("\n--- Student %d ---\n", count + 1);
        print_student(&student);

        count++;
    }

    if (count == 0) {
        printf("No students found.\n");
    }

    return 0;
}

static int find_student(int fd) {
    Student_t student;
    int search_id;
    ssize_t bytes_read;

    printf("Enter ID to find: ");

    if (scanf("%d", &search_id) != 1)   {
        printf("Invalid ID.\n");
        return -1;
    }

    /*
     * Start scanning from the beginning.
     */
    if (lseek(fd, 0, SEEK_SET) == (off_t)-1)    {
        perror("lseek");
        return -1;
    }

    while (1)   {
        bytes_read = read(fd, &student, sizeof(Student_t));

        if (bytes_read == 0) {
            break;
        }

        if (bytes_read != (ssize_t)sizeof(Student_t)) {
            printf("Error: corrupted or incomplete record.\n");
            return -1;
        }

        if (student.id == search_id) {
            printf("\nStudent found:\n");
            print_student(&student);
            return 0;
        }
    }

    printf("Student with ID %d not found.\n", search_id);

    return 0;
}

