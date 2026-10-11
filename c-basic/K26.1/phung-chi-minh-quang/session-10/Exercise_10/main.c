#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MIN_CAPACITY 1
typedef struct
{
    int id;
    char name[50];
    float gpa;
} Student;

int add_student(Student **arr, int *count, int *capacity)
{
    // Nhập thông tin học sinh
    Student s;
    printf("Nhập ID: ");
    if (scanf("%d", &s.id) != 1)
    {
        printf("Lỗi: Dữ liệu nhập sai\n");
        while (getchar() != '\n');//xóa hết các kí tự còn sót trong stdin
        return 0;
    }
    getchar();
    printf("Nhập tên: ");
    fgets(s.name, sizeof(s.name), stdin);
    int len = strlen(s.name);
    if (len > 0 && s.name[len - 1] == '\n')
    {
        s.name[len - 1] = '\0';
    }

    printf("Nhập GPA: ");
    if (scanf("%f", &s.gpa) != 1)
    {
        printf("Lỗi: Dữ liệu nhập sai\n");
        while (getchar() != '\n');//xóa hết các kí tự còn sót trong stdin
        return 0;
    }
    while (getchar() != '\n');//xóa hết các kí tự còn sót trong stdin sau khi nhập gpt thành công

    
    if (*count + 1 > *capacity)
    {
        // Mở rộng thêm capacity
        *capacity = *capacity * 2;
        Student *tmp = realloc(*arr, *capacity * sizeof(Student));
        if (tmp == NULL)
        {
            printf("Lỗi: Không đủ bộ nhớ RAM\n");
            return 0;
        }
        *arr = tmp; // gán lại địa chỉ cho arr sau khi realloc thành công
    }
    *count = *count + 1;
    (*arr)[*count - 1] = s;
    return 1;
}

void print_student(Student *arr, int count)
{
    if (count == 0)
    {
        printf("Danh sách trống\n");
        return;
    }

    printf("Danh sách (%d học sinh)\n", count);
    for (int i = 0; i < count; i++)
    {
        printf("%d. %s, %.2f\n", arr[i].id, arr[i].name, arr[i].gpa);
    }
    return;
}

int delete_student(Student *arr, int *count, int deleted_id)
{
    // Tìm index của học sinh có id = deleted_id
    int target_index = -1;
    for (int i = 0; i < *count; i++)
    {
        if (arr[i].id == deleted_id)
        {
            target_index = i;
            break;
        }
    }

    if (target_index == -1)
    {
        return 0; // Không tìm thấy học sinh có id = deleted_id
    }

    // Xóa học sinh bằng cách chuyển phần tử sau lên trước 1 vị trí
    for (int i = target_index; i < *count - 1; i++)
    {
        arr[i] = arr[i + 1];
    }
    *count = *count - 1;
    return 1;
}

int main(void)
{
    int count = 0;                                     // lưu số lượng học sinh hiện có
    int capacity = MIN_CAPACITY;                       // lưu số lượng học sinh có thể chứa
    Student *arr = malloc(capacity * sizeof(Student)); // lưu địa chỉ của danh sách học sinh
    if (arr == NULL)
    {
        printf("Lỗi: Không đủ bộ nhớ RAM\n");
        return 1;
    }

    int choose = 0;
    do
    {
        printf("\n");
        printf("====== MENU ======\n");
        printf("1. Thêm học sinh\n");
        printf("2. In danh sách\n");
        printf("3. Xóa học sinh\n");
        printf("4. Thoát\n");
        printf("Chọn: ");

        if (scanf("%d", &choose) != 1 || choose < 1 || choose > 4)
        {
            printf("Lỗi: Vui lòng nhập số nguyên dương trong 1 đến 4!\n");
            while (getchar() != '\n');//xóa hết các kí tự còn sót trong stdin
            continue;
        }

        switch (choose)
        {
        case 1:
            add_student(&arr, &count, &capacity);
            break;
        case 2:
            print_student(arr, count);
            break;
        case 3:
            printf("Nhập ID cần xóa: ");
            int deleted_id;
            if (scanf("%d", &deleted_id) != 1)
            {
                printf("Lỗi: Dữ liệu nhập sai\n");
                while (getchar() != '\n');//xóa hết các kí tự còn sót trong stdin
                break;
            }
            while (getchar() != '\n');

            if (delete_student(arr, &count, deleted_id))
            {
                printf("Học sinh đã xóa\n");
            }
            else
            {
                printf("Không tìm thấy học sinh có id = %d\n", deleted_id);
            }
            break;
        case 4:
            printf("Thoát. ");
            break;
        }
    } while (choose != 4);

    free(arr);
    arr = NULL;
    printf("Đã giải phóng bộ nhớ\n");
    return 0;
}