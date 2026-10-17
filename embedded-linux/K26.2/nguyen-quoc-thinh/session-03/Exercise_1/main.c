#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define FILENAME "students.dat"

typedef struct {
    int   id;
    char  name[64];
    int   age;
    float gpa;
} Student;

static int fd = -1;

static void open_data_file(void)
{
    fd = open(FILENAME, O_RDWR | O_CREAT, 0644);

    if (fd < 0) {
        perror("open students.dat");
        exit(EXIT_FAILURE);
    }
}

static void flush_stdin(void)
{
    int c;

    while ((c = getchar()) != '\n' && c != EOF) {
    }
}

static void add_student(void)
{
    Student s;
    memset(&s, 0, sizeof(s));

    printf("Nhap ID: ");
    scanf("%d", &s.id);
    flush_stdin();

    printf("Nhap ten: ");
    fgets(s.name, sizeof(s.name), stdin);

    size_t len = strlen(s.name);
    if (len > 0 && s.name[len - 1] == '\n') {
        s.name[len - 1] = '\0';
    }

    printf("Nhap tuoi: ");
    scanf("%d", &s.age);

    printf("Nhap GPA: ");
    scanf("%f", &s.gpa);
    flush_stdin();

    off_t end = lseek(fd, 0, SEEK_END);

    if (end < 0) {
        perror("lseek");
        return;
    }

    ssize_t written = write(fd, &s, sizeof(s));

    if (written != sizeof(s)) {
        fprintf(stderr, "Loi ghi file\n");
        return;
    }

    printf("Da them sinh vien ID=%d thanh cong.\n", s.id);
}

static void print_student(const Student *s)
{
    printf("ID=%-6d Ten=%-20s Tuoi=%-3d GPA=%.2f\n",
           s->id,
           s->name,
           s->age,
           s->gpa);
}

static void list_students(void)
{
    if (lseek(fd, 0, SEEK_SET) < 0) {
        perror("lseek");
        return;
    }

    Student s;
    int count = 0;
    ssize_t n;

    while ((n = read(fd, &s, sizeof(s))) == (ssize_t)sizeof(s)) {
        print_student(&s);
        count++;
    }

    if (n < 0) {
        perror("read");
    }

    if (count == 0) {
        printf("(Danh sach rong)\n");
    }
}

static void find_student(void)
{
    int target;

    printf("Nhap ID can tim: ");
    scanf("%d", &target);
    flush_stdin();

    if (lseek(fd, 0, SEEK_SET) < 0) {
        perror("lseek");
        return;
    }

    Student s;
    ssize_t n;

    while ((n = read(fd, &s, sizeof(s))) == (ssize_t)sizeof(s)) {
        if (s.id == target) {
            printf("Tim thay:\n");
            print_student(&s);
            return;
        }
    }

    if (n < 0) {
        perror("read");
    }

    printf("Khong tim thay sinh vien ID=%d\n", target);
}

static void print_menu(void)
{
    printf("\n===== QUAN LY SINH VIEN =====\n");
    printf("1. Them sinh vien\n");
    printf("2. Liet ke tat ca\n");
    printf("3. Tim sinh vien theo ID\n");
    printf("4. Thoat\n");
    printf("Chon: ");
}

int main(void)
{
    open_data_file();

    int choice;

    while (1) {
        print_menu();

        if (scanf("%d", &choice) != 1) {
            flush_stdin();
            continue;
        }

        flush_stdin();

        switch (choice) {
            case 1:
                add_student();
                break;

            case 2:
                list_students();
                break;

            case 3:
                find_student();
                break;

            case 4:
                close(fd);
                printf("Tam biet!\n");
                return 0;

            default:
                printf("Lua chon khong hop le.\n");
        }
    }
}