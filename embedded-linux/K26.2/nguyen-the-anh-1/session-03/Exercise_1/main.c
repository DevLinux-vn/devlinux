#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define DATA_FILE "students.dat"

typedef struct {
    int   id;
    char  name[64];
    int   age;
    float gpa;
} Student;

/* Đọc 1 dòng số nguyên từ stdin an toàn (tránh lỗi khi user gõ chữ) */
static int read_int(const char *prompt)
{
    char buf[64];
    int value;
    printf("%s", prompt);
    fgets(buf, sizeof(buf), stdin);
    sscanf(buf, "%d", &value);
    return value;
}

static float read_float(const char *prompt)
{
    char buf[64];
    float value;
    printf("%s", prompt);
    fgets(buf, sizeof(buf), stdin);
    sscanf(buf, "%f", &value);
    return value;
}

static void read_line(const char *prompt, char *out, size_t max_len)
{
    printf("%s", prompt);
    fgets(out, (int)max_len, stdin);
    /* Xoá ký tự newline fgets để lại cuối chuỗi */
    size_t len = strlen(out);
    if (len > 0 && out[len - 1] == '\n') {
        out[len - 1] = '\0';
    }
}

static void add_student(void)
{
    Student s;
    memset(&s, 0, sizeof(s));

    s.id = read_int("Nhap ID: ");
    read_line("Nhap ten: ", s.name, sizeof(s.name));
    s.age = read_int("Nhap tuoi: ");
    s.gpa = read_float("Nhap GPA: ");

    /* O_APPEND dam bao ghi noi vao cuoi file, khong ghi de */
    int fd = open(DATA_FILE, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd == -1) {
        perror("open");
        return;
    }

    ssize_t n = write(fd, &s, sizeof(Student));
    if (n != (ssize_t)sizeof(Student)) {
        perror("write");
    } else {
        printf("Da them sinh vien ID=%d\n", s.id);
    }

    close(fd);
}

static void list_students(void)
{
    int fd = open(DATA_FILE, O_RDONLY);
    if (fd == -1) {
        /* Chua co file nghia la chua co record nao, khong phai loi nghiem trong */
        printf("Chua co du lieu sinh vien.\n");
        return;
    }

    Student s;
    int count = 0;
    printf("%-6s %-20s %-6s %-6s\n", "ID", "Ten", "Tuoi", "GPA");

    while (1) {
        ssize_t n = read(fd, &s, sizeof(Student));
        if (n == 0) break;              /* het file */
        if (n != (ssize_t)sizeof(Student)) {
            perror("read");
            break;
        }
        printf("%-6d %-20s %-6d %-6.2f\n", s.id, s.name, s.age, s.gpa);
        count++;
    }

    if (count == 0) {
        printf("Danh sach rong.\n");
    }

    close(fd);
}

static void find_student(void)
{
    int target_id = read_int("Nhap ID can tim: ");

    int fd = open(DATA_FILE, O_RDONLY);
    if (fd == -1) {
        printf("Chua co du lieu sinh vien.\n");
        return;
    }

    Student s;
    int found = 0;

    while (1) {
        ssize_t n = read(fd, &s, sizeof(Student));
        if (n == 0) break;
        if (n != (ssize_t)sizeof(Student)) {
            perror("read");
            break;
        }
        if (s.id == target_id) {
            printf("Tim thay: ID=%d Ten=%s Tuoi=%d GPA=%.2f\n",
                   s.id, s.name, s.age, s.gpa);
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("Khong tim thay sinh vien ID=%d\n", target_id);
    }

    close(fd);
}

static void print_menu(void)
{
    printf("\n===== QUAN LY SINH VIEN =====\n");
    printf("1. Them sinh vien\n");
    printf("2. Liet ke tat ca\n");
    printf("3. Tim theo ID\n");
    printf("4. Thoat\n");
    printf("Chon: ");
}

int main(void)
{
    int choice;
    int running = 1;

    while (running) {
        print_menu();
        choice = read_int("");

        switch (choice) {
            case 1: add_student();    break;
            case 2: list_students();  break;
            case 3: find_student();   break;
            case 4:
                printf("Tam biet!\n");
                running = 0;
                break;
            default:
                printf("Lua chon khong hop le.\n");
        }
    }

    return 0;
}
