//Struct lưu thông tin học sinh
#include <stdio.h>
#include <string.h>

typedef struct
{
    int id;
    char name[50];
    float gpa;
} Student;

int main(void)
{
    Student s1;
    Student *ptr = &s1; //Thử dùng con trỏ để truy cập struct Student

    printf("Nhập ID: ");
    if(scanf("%d", &s1.id) != 1){
        printf("Nhập sai!\n");
        return 1;
    }
    if(s1.id < 0){
        printf("ID phải là số nguyên dương!\n");
        return 1;
    }

    while(getchar() != '\n');//Lấy hết kí tự còn sót trong buffer cho đến khi gặp gặp kí tự Enter \n trong stdin
    
    printf("Nhập tên: ");
    fgets(ptr->name, 50, stdin);
    ptr->name[strlen(ptr->name) - 1] = '\0';//Để loại bỏ kí tự Enter \n ở cuối chuỗi.
    
    printf("Nhập GPA: ");
    if(scanf("%f", &ptr->gpa) != 1){
        printf("Nhập sai!\n");
        return 1;
    }
    if(ptr->gpa < 0.0 || ptr->gpa > 4.0){
        printf("GPA phải là giá trị từ 0.0 đến 4.0!\n");
        return 1;
    }


    printf("Thông tin: ");
    printf("ID= %d, ", ptr->id);
    printf("Tên= %s, ", ptr->name);
    printf("GPA= %.2f\n", ptr->gpa);

    return 0;
}