// Cấp phát bộ nhớ cho struct

// Viết chương trình C:
// 1. Định nghĩa struct `Student`
// 2. Dùng `malloc()` để cấp phát cho 1 biến `Student`
// 3. Nhập thông tin
// 4. In thông tin
// 5. Giải phóng bằng `free()`

// **Ví dụ:**
// ```
// Nhập ID: 1
// Nhập tên: Nguyen Van A
// Nhập GPA: 3.5
// Thông tin: ID=1, Tên=Nguyen Van A, GPA=3.50
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    int id;
    char name[50];
    float gpa;
} Student;

int main(void)
{
    Student *s = malloc(sizeof(Student));
    if (s == NULL)
    {
        printf("Lỗi: Không đủ bộ nhớ RAM\n");
        return 1;
    }

    printf("Nhập ID: ");
    if (scanf("%d", &s->id) != 1)
    {
        printf("Lỗi: Dữ liệu nhập sai\n");
        free(s);
        return 1;
    }
    getchar();
    printf("Nhập tên: ");
    fgets(s->name, sizeof(s->name), stdin);
    int len = strlen(s->name);
    if (len > 0 && s->name[len - 1] == '\n')
    {
        s->name[len - 1] = '\0';
    }
    printf("Nhập GPA: ");
    if (scanf("%f", &s->gpa) != 1)
    {
        printf("Lỗi: Dữ liệu nhập sai\n");
        free(s);
        return 1;
    }
    while (getchar() != '\n');

    printf("Thông tin: ");
    printf("ID=%d, Tên=%s, GPA=%.2f\n", s->id, s->name, s->gpa);

    // Giải phóng bộ nhớ
    free(s);
    s = NULL;
    return 0;
}