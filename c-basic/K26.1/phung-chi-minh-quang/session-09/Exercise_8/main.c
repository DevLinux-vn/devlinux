// Hàm với struct

// Viết chương trình C:
// 1. Viết hàm `void input(Student *s)` để nhập thông tin
// 2. Viết hàm `void print(Student s)` để in thông tin
// 3. Viết hàm `float average(Student arr[], int n)` để tính GPA trung bình
// 4. Trong `main()`:
//    - Nhập 3 học sinh
//    - In danh sách
//    - Tính GPA trung bình

// **Ví dụ:**
// ```
// Nhập 3 học sinh và tính GPA trung bình: 3.50
// ```

#include <stdio.h>
#include <string.h>

typedef struct
{
    int id;
    char name[50];
    float gpa;
} Student;

void input(Student *s)
{
    printf("ID=");
    scanf("%d", &s->id);
    while (getchar() != '\n')
        ;

    printf("Tên=");
    fgets(s->name, sizeof(s->name), stdin);
    s->name[strlen(s->name) - 1] = '\0';

    printf("GPA=");
    scanf("%f", &s->gpa);
}

void print(Student s)
{
    printf("ID= %d, ", s.id);
    printf("Tên= %s, ", s.name);
    printf("GPA= %.2f\n", s.gpa);
}

float average(Student arr[], int n)
{
    float sum = 0;
    for (int i = 0; i < n; i++)
    {
        sum += arr[i].gpa;
    }
    return sum / n;
}

int main(void){
    Student student_arr[3];
    int n = 3;

    printf("Nhập %d học sinh và tính GPA trung bình: \n", n);
    for(int i = 0; i < n; i++){
        printf("Nhập thông tin học sinh %d: \n", i + 1);
        input(&student_arr[i]);
    }

    //In danh sách student_arr
    for(int i = 0; i < n; i++){
        printf("Thông tin học sinh %d: \n", i + 1);
        print(student_arr[i]);
    }
    //Tính GPA trung bình
    float gpa_average = average(student_arr, n);
    printf("GPA trung bình: %.2f\n", gpa_average);
    return 0;
}