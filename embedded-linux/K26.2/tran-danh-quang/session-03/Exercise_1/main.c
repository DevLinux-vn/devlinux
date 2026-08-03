#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#define DB_FILE "students.dat"
#define MAX_NAME_LEN 64
#define SUCCESS 0
#define ERR_IO -1
#define ERR_INPUT -2
#define ERR_NOT_FOUND -3

typedef struct {
    int   id;
    char  name[MAX_NAME_LEN];
    int   age;
    float gpa;
} Student;

int clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    return SUCCESS;
}

int get_int_input(const char *prompt, int *out_val) {
    while (1) {
        printf("%s", prompt);
        int res = scanf("%d", out_val);
        if (res == EOF) {
            return ERR_IO;
        }
        if (res == 1) {
            clear_input_buffer();
            return SUCCESS;
        } else {
            clear_input_buffer();
            printf("Invalid input. Please enter an integer.\n");
        }
    }
}

int get_float_input(const char *prompt, float *out_val) {
    while (1) {
        printf("%s", prompt);
        int res = scanf("%f", out_val);
        if (res == EOF) {
            return ERR_IO;
        }
        if (res == 1) {
            clear_input_buffer();
            return SUCCESS;
        } else {
            clear_input_buffer();
            printf("Invalid input. Please enter a number.\n");
        }
    }
}

int get_string_input(const char *prompt, char *out_str, int max_len) {
    printf("%s", prompt);
    if (fgets(out_str, max_len, stdin) == NULL) {
        return ERR_IO;
    }
    size_t len = strlen(out_str);
    if (len > 0 && out_str[len - 1] == '\n') {
        out_str[len - 1] = '\0';
    } else {
        clear_input_buffer();
    }
    return SUCCESS;
}

int add_student(void) {
    Student s;
    if (get_int_input("Enter student ID: ", &s.id) != SUCCESS) return ERR_INPUT;
    if (get_string_input("Enter student name: ", s.name, MAX_NAME_LEN) != SUCCESS) return ERR_INPUT;
    if (get_int_input("Enter student age: ", &s.age) != SUCCESS) return ERR_INPUT;
    if (get_float_input("Enter student GPA: ", &s.gpa) != SUCCESS) return ERR_INPUT;

    int fd;
    do {
        fd = open(DB_FILE, O_WRONLY | O_CREAT | O_APPEND, 0644);
    } while (fd == -1 && errno == EINTR);

    if (fd == -1) {
        perror("open failed");
        return ERR_IO;
    }

    ssize_t bytes_written;
    do {
        bytes_written = write(fd, &s, sizeof(Student));
    } while (bytes_written == -1 && errno == EINTR);

    if (bytes_written != sizeof(Student)) {
        perror("write failed");
        close(fd);
        return ERR_IO;
    }

    if (close(fd) == -1) {
        perror("close failed");
        return ERR_IO;
    }
    printf("Student added successfully.\n");
    return SUCCESS;
}

int list_students(void) {
    int fd;
    do {
        fd = open(DB_FILE, O_RDONLY);
    } while (fd == -1 && errno == EINTR);

    if (fd == -1) {
        if (errno == ENOENT) {
            printf("No records found.\n");
            return SUCCESS;
        }
        perror("open failed");
        return ERR_IO;
    }

    Student s;
    ssize_t bytes_read;
    printf("--- Student List ---\n");
    while (1) {
        do {
            bytes_read = read(fd, &s, sizeof(Student));
        } while (bytes_read == -1 && errno == EINTR);
        
        if (bytes_read != sizeof(Student)) {
            break;
        }
        printf("ID: %d, Name: %s, Age: %d, GPA: %.2f\n", s.id, s.name, s.age, s.gpa);
    }

    if (bytes_read == -1) {
        perror("read failed");
    }

    if (close(fd) == -1) {
        perror("close failed");
        return ERR_IO;
    }
    printf("--------------------\n");
    return (bytes_read == -1) ? ERR_IO : SUCCESS;
}

int find_student(int id) {
    int fd;
    do {
        fd = open(DB_FILE, O_RDONLY);
    } while (fd == -1 && errno == EINTR);

    if (fd == -1) {
        if (errno == ENOENT) {
            printf("No records found.\n");
            return ERR_NOT_FOUND;
        }
        perror("open failed");
        return ERR_IO;
    }

    Student s;
    ssize_t bytes_read;
    int found = 0;
    while (1) {
        do {
            bytes_read = read(fd, &s, sizeof(Student));
        } while (bytes_read == -1 && errno == EINTR);
        
        if (bytes_read != sizeof(Student)) {
            break;
        }
        
        if (s.id == id) {
            printf("Found - ID: %d, Name: %s, Age: %d, GPA: %.2f\n", s.id, s.name, s.age, s.gpa);
            found = 1;
            break;
        }
    }

    if (bytes_read == -1) {
        perror("read failed");
    }

    if (close(fd) == -1) {
        perror("close failed");
        return ERR_IO;
    }

    if (!found) {
        printf("Student with ID %d not found.\n", id);
        return ERR_NOT_FOUND;
    }
    return SUCCESS;
}

int print_menu(void) {
    printf("\nMenu:\n");
    printf("1. Add student\n");
    printf("2. List all students\n");
    printf("3. Find student by ID\n");
    printf("4. Exit\n");
    return SUCCESS;
}

int main(void) {
    int choice;
    int running = 1;

    while (running) {
        print_menu();
        if (get_int_input("Enter choice: ", &choice) != SUCCESS) {
            if (feof(stdin)) {
                printf("\nEOF detected. Exiting...\n");
                break;
            }
            continue;
        }

        switch (choice) {
            case 1:
                add_student();
                break;
            case 2:
                list_students();
                break;
            case 3: {
                int id;
                if (get_int_input("Enter student ID to find: ", &id) == SUCCESS) {
                    find_student(id);
                } else {
                    if (feof(stdin)) running = 0;
                }
                break;
            }
            case 4:
                running = 0;
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice. Try again.\n");
        }
    }
    return 0;
}
