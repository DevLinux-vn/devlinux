// Mảng struct
// Viết chương trình C:
// 1. Định nghĩa struct `Student`
// 2. Khai báo Mảng 3 học sinh
// 3. Nhập thông tin 3 học sinh
// 4. In danh sách và tính GPA trung bình
#include <stdio.h>
#include <string.h>

// Định nghĩa struct Student
typedef struct
{
    int id;
    char name[50];
    float gpa;
} Student;

int main(void)
{
    Student student_arr[3];
    Student *ptr = student_arr;
    float sum = 0;

    printf("Nhập 3 học sinh:\n");

    for (int i = 0; i < 3; i++)
    {
        printf("Học sinh %d:\n", i + 1);

        printf("ID=");
        scanf("%d", &ptr->id);
        while (getchar() != '\n')
            ;

        printf("Tên=");
        fgets(ptr->name, sizeof(ptr->name), stdin);
        ptr->name[strlen(ptr->name) - 1] = '\0';

        printf("GPA=");
        scanf("%f", &ptr->gpa);


        sum += ptr->gpa;
        ptr++;
    }

    //In thông tin 3 học sinh
    for (int i = 0; i < 3; i++)
    {
        printf("Học sinh %d: ", i + 1);
        printf("ID= %d, ", student_arr[i].id);
        printf("Tên= %s, ", student_arr[i].name);
        printf("GPA= %f\n", student_arr[i].gpa);
    }

    // In GPA trung bình
    printf("GPA trung bình: %.2f\n", sum / 3);

    return 0;
}