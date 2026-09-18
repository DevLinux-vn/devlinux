#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define INDEX(_ARRAY_) (sizeof(_ARRAY_)/sizeof(_ARRAY_[0]))
typedef enum{
    ADD_STUDENT = 1,
    LIST_ALL_STUDENT,
    FIND_STUDENT,
    EXIT_CHOICE,
} e_menu_t;

#define MIN_ID 1
#define MAX_ID 100

#define MIN_AGE 18
#define MAX_AGE 50

#define MIN_GPA 0.0
#define MAX_GPA 4.0

typedef struct {
    int   id;
    char  name[64];
    int   age;
    float gpa;
} Student;

int add_student(Student *student)
{
    struct query_student_field {
        char *question;
        char *scanf_pattern;
        void *arg;
    };
    Student temp;
    struct query_student_field query_field[] = {
        {"Which is the ID of the student?\n", "%d", &temp.id},
        {"What is the name of the student?\n", "%s", &temp.name},
        {"How old is the student?\n", "%d", &temp.age},
        {"How many GPA score of the student ?\n", "%f", &temp.gpa},
    };
    memset(temp.name, 0, sizeof(temp.name));
    for(uint32_t i = 0; i < INDEX(query_field); i++)
    {
        printf("%s", query_field[i].question);
        if (scanf(query_field[i].scanf_pattern, query_field[i].arg) != 1) {
            printf("Error: Invalid input argument\n");
            i--;
        }
        /* Check input age */
        if(query_field[i].arg == &temp.age && (temp.age < MIN_AGE || temp.age > MAX_AGE)) {
            printf("Error: Student age input is invalid, range from %d to %d \n", MIN_AGE, MAX_AGE);
            i--;
        }
        /* Check input GPA */
        else if(query_field[i].arg == &temp.gpa && (temp.gpa > MIN_GPA  || temp.gpa > MAX_GPA))
        {
            printf("Error: Student GPA input is invalid, range from %.2f to %.2f \n", MIN_GPA, MAX_GPA);
            i--;
        }
        /* Check input student ID */
        else if(query_field[i].arg == &temp.id && (temp.id > MIN_ID  || temp.id > MAX_ID))
        {
            printf("Error: Student ID input is invalid, range from %d to %d \n", MIN_ID, MAX_ID);
            i--;
        }

    }
    *student = temp;
    return 0;
}

int list_student()
{
    Student student;

    int fd = open("students.dat", O_RDONLY);
    if(fd == -1)
    {
        printf("Error of opening file\n");
        return 1;
    }
    // Reset file offset to the beginning
    lseek(fd, 0, SEEK_SET);

    ssize_t count = read(fd, (void*)&student, sizeof(student));
    if(!count) {
        printf("Error: EOF\n");
    }
    else {
        printf("Read %lu byte\n", count);
    }
    return 0;
}

int find_student(int id)
{
    (void)id;
    return 0;

}

e_menu_t print_menu()
{
    e_menu_t menu;
    printf("Menu\n");
    printf("1. Add student\n");
    printf("2. List all students\n");
    printf("3. Find student by ID\n");
    printf("4. Exit\n");
    if(scanf("%d", &menu) != 1) {
        printf("Error: Invalid input argument\n");
        return 1;
    }
    return menu;
}

static int write_to_file(Student student)
{
    int fd = open("students.dat", O_CREAT | O_WRONLY | O_APPEND, 0644);
    if(fd == -1)
    {
        printf("Error of opening file\n");
        return 1;
    }
    ssize_t count = write(fd, (void*)&student, sizeof(student));
    if(count != sizeof(Student)) {
        printf("Error: write data mismatch, byte written:%lu\n", count);
        return 1;
    }

    close(fd);
    return 0;
}

int main()
{
    int err = 0;
    e_menu_t menu;
    while(1)
    {
        if(err == 0) {
            menu = print_menu();
        }
        else {
            printf("Error: Fail to process menu operation\n");
            return 1;
        }
        switch(menu) {
            case ADD_STUDENT:
                Student student;
                err = add_student(&student);
                if(err == 0) {
                   err = write_to_file(student);
                }
                break;
            case LIST_ALL_STUDENT:
                break;
            case FIND_STUDENT:
                break;
            case EXIT_CHOICE:
                printf("Exit\n");
                return 0;
            default: 
                printf("Error: Invalid choice\n");
                continue;
        }

    }
    return 0;
}
