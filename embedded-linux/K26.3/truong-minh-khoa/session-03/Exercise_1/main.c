#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

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

#define STUDENT_NAME_MAX 64

#define STUDENT_DATA_FILE_NAME "students.dat"

static ssize_t read_partial(int fd, const void *buf, size_t total_byte);
static ssize_t write_partial(int fd, const void *buf, size_t total_byte);

int student_id_list[MAX_ID - MIN_ID] = {0};

typedef struct {
    int   id;
    char  name[STUDENT_NAME_MAX];
    int   age;
    float gpa;
} Student;

#ifdef PRELOAD_STUDENT_LIST
Student student_test_list[] = {
    {1	, "Phuong", 35, 2.6},
    {2	, "Lan"	  , 19, 3.1},
    {3	, "Minh"  , 25, 3.2},
    {4	, "Tien"  , 31, 3.2},
    {5	, "Nguyen", 46, 2.0},
    {7	, "Van"	  , 23, 2.7},
    {8	, "Phat"  , 23, 1.2},
    {9	, "Nghia" , 38, 1.9},
    {10	, "Uc"	  , 47, 3.8},
    {11	, "Man"	  , 39, 2.2},
};
#endif

int add_student(Student *student)
{
    int err;
    struct query_student_field {
        char *question;
        char *scanf_pattern;
        void *arg;
    };
    Student temp;
    struct query_student_field query_field[] = {
        {"Which is the ID of the student?\n"    , "%d", &temp.id    },
        {"What is the name of the student?\n"   , "%s", &temp.name  },
        {"How old is the student?\n"            , "%d", &temp.age   },
        {"How many GPA score of the student ?\n", "%f", &temp.gpa   },
    };
    memset(temp.name, 0, sizeof(temp.name));
    for(long unsigned int i = 0; i < INDEX(query_field); i++) {
        printf("%s", query_field[i].question);
        err = scanf(query_field[i].scanf_pattern, query_field[i].arg);
        if (err != 1) {
            printf("Error: Invalid input argument, err:%d\n", err);
            while (getchar() != '\n');
            i--;
            continue;
        }
        /* Check input age */
        if(query_field[i].arg == &temp.age && (temp.age < MIN_AGE || temp.age > MAX_AGE)) {
            printf("Error: Student age input is invalid, range from %d to %d\n", MIN_AGE, MAX_AGE);
            i--;
            continue;
        }
        /* Check input GPA */
        else if(query_field[i].arg == &temp.gpa && (temp.gpa < MIN_GPA  || temp.gpa > MAX_GPA))
        {
            printf("Error: Student GPA input is invalid, range from %.2f to %.2f\n", MIN_GPA, MAX_GPA);
            i--;
            continue;
        }
        /* Check input student ID */
        else if(query_field[i].arg == &temp.id)
        {
            if(temp.id < MIN_ID  || temp.id > MAX_ID) {
                printf("Error: Student ID input is invalid, range from %d to %d\n", MIN_ID, MAX_ID);
                i--;
                continue;
            }
            else if(student_id_list[temp.id] == 1)
            {
                printf("Error: Student ID %d is already assigned, please select another\n", temp.id);
                i--;
                continue;
            }
            else {
                student_id_list[temp.id] = 1;
            }
        }

    }
    *student = temp;
    return 0;
}

int list_all_student()
{
    Student student;

    int fd = open(STUDENT_DATA_FILE_NAME, O_RDONLY);
    if(fd == -1)
    {
        printf("Error of opening file\n");
        return 1;
    }

    // Reset file offset to the beginning
    off_t result = lseek(fd, 0, SEEK_SET);
    if (result == (off_t)-1) {
        perror("lseek failed");
        if(close(fd) == -1) {
            perror("Error: Cannot close file\n");
            return 1;
        }
        return 1;
    }

    while(1) {
        ssize_t count = read_partial(fd, (void*)&student, sizeof(student));
        if(count == 0) {
            printf("EOF\n");
            if(close(fd) == -1) {
                perror("Error: Cannot close file\n");
                return 1;
            }
            return 0;
        }
        else if (count < 0) {
            printf("Error: Fail to read data\n");
            if(close(fd) == -1) {
                perror("Error: Cannot close file\n");
                return 1;
            }
            return 1;
        }
        else {
            printf("---------------\n");
            printf("Student:\n");
            printf("id: %d,\t name: %s,\t age: %d,\t gpa: %.2f\n", student.id, student.name, student.age, student.gpa);
        }
    }

    if(close(fd) == -1) {
        perror("Error: Cannot close file\n");
        return 1;
    }

    return 0;
}

int find_student()
{
    int err;
    int find_student_id;
    Student student;

    printf("Input student ID to find:\n");
    err = scanf("%d", &find_student_id);
    while (err != 1) {
        printf("Error: Invalid input argument, err:%d\n", err);
        while (getchar() != '\n');
        err = scanf("%d", &find_student_id);
    }


    int fd = open(STUDENT_DATA_FILE_NAME, O_RDONLY);
    if(fd == -1)
    {
        perror("Error of opening file\n");
        return 1;
    }

    // Reset file offset to the beginning
    off_t result = lseek(fd, 0, SEEK_SET);
    if (result == (off_t)-1) {
        perror("lseek failed");
        if(close(fd) == -1) {
            perror("Error: Cannot close file\n");
            return 1;
        }
        return 1;
    }


    while(1) {
        ssize_t count = read_partial(fd, (void*)&student, sizeof(student));
        if(count == 0) {
            printf("Not found student\n");
            if(close(fd) == -1) {
                perror("Error: Cannot close file\n");
                return 1;
            }
            return 0;
        }
        else if (count < 0) {
            printf("Error: Fail to read data\n");
            if(close(fd) == -1) {
                perror("Error: Cannot close file\n");
                return 1;
            }
            return 1;
        }
        else if (student.id == find_student_id){
            printf("---------------\n");
            printf("Student has been found:");
            printf("id: %d,\t name: %s,\t age: %d,\t gpa: %.2f\n", student.id, student.name, student.age, student.gpa);
            break;
        }
        else {}
    }
    
    if(close(fd) == -1) {
        perror("Error: Cannot close file\n");
        return 1;
    }

    return 0;
}

static ssize_t read_partial(int fd, const void *buf, size_t total_byte)
{
    size_t byte_read = 0;
    while (byte_read < total_byte) {
        ssize_t read_chunk = read(fd, (void*)buf + byte_read, total_byte - byte_read);
        if(read_chunk < 0) {
            if(errno == EINTR) {
                continue;
            }
            perror("Error: Fail to read chunk data");
            return -1;
        }
        else if(read_chunk == 0) {
            return 0;
        }
        byte_read += read_chunk;
    };
    return byte_read;
}

static ssize_t write_partial(int fd, const void *buf, size_t total_byte)
{
    size_t byte_written = 0;
    while (byte_written < total_byte) {
        ssize_t written_chunk = write(fd, (void*)buf + byte_written, total_byte - byte_written);
        if(written_chunk < 0) {
            if(errno == EINTR) {
                continue;
            }
            perror("Error: Fail to write chunk data");
            return -1;
        }
        else if(written_chunk == 0) {
            return 0;
        }
        byte_written += written_chunk;
    };
    return byte_written;
}


e_menu_t print_menu()
{
    e_menu_t menu;
    int temp;
    printf("Menu\n");
    printf("1. Add student\n");
    printf("2. List all students\n");
    printf("3. Find student by ID\n");
    printf("4. Exit\n");
    if(scanf("%d", (int*)&temp) != 1) {
        printf("Error: Invalid input argument\n");
        return 1;
    }
    menu = (e_menu_t)temp;
    return menu;
}

static int write_to_file(Student student)
{
    int fd = open(STUDENT_DATA_FILE_NAME, O_CREAT | O_WRONLY | O_APPEND, 0644);
    if(fd == -1)
    {
        perror("Error of opening file\n");
        return 1;
    }

    if (lseek(fd, 0, SEEK_END) == (off_t)-1) {
        perror("lseek");
        if(close(fd) == -1) {
            perror("Error: Cannot close file\n");
            return 1;
        }
        return 1;
    }
    ssize_t count = write_partial(fd, (void*)&student, sizeof(student));
    if(count < 0) {
        perror("Error: Can not write file\n");
        if(close(fd) == -1) {
            perror("Error: Cannot close file\n");
            return 1;
        }
        return 1;
    }
    else if(count != (ssize_t)sizeof(student))
    {
        perror("Error: write size mismatch with expected\n");
        if(close(fd) == -1) {
            perror("Error: Cannot close file\n");
            return 1;
        }
        return 1;
    }
    printf("Write student info to %s successfully\n", STUDENT_DATA_FILE_NAME);
    printf("id: %d,\t name: %s,\t age: %d,\t gpa: %.2f\n", student.id, student.name, student.age, student.gpa);

    if(close(fd) == -1) {
        perror("Error: Cannot close file\n");
        return 1;
    }
    return 0;
}

#ifdef PRELOAD_STUDENT_LIST
static int load_student_list()
{
    int err = 0;
    for(long unsigned int i = 0; i < INDEX(student_test_list); i++) {
        Student student = student_test_list[i];
        err = write_to_file(student);
        student_id_list[student.id] = 1;
        if(err) {
            printf("Error: fail to write to %s\n", STUDENT_DATA_FILE_NAME);
            return 1;
        }
    }
    return 0;
}
#endif

int main()
{
    int err = 0;
    e_menu_t menu;
    Student student;
#ifdef PRELOAD_STUDENT_LIST
    err = load_student_list();
#endif
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
                err = add_student(&student);
                if(err == 0) {
                   err = write_to_file(student);
                }
                break;
            case LIST_ALL_STUDENT:
                err = list_all_student();
                break;
            case FIND_STUDENT:
                err = find_student();
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
