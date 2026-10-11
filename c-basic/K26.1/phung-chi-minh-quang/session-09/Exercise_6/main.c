//Con trỏ và struct
#include <stdio.h>
#include <string.h>

//Định nghĩa struct Student
typedef struct{
    int id;
    char name[50];
    float gpa;
}Student;
int main(void){
    Student s1;
    Student *ptr = &s1;

    printf("Nhập ID: ");
    scanf("%d", &ptr->id);
    while(getchar() != '\n');

    printf("Nhập tên: ");
    fgets(ptr->name, sizeof(ptr->name), stdin);
    ptr->name[strlen(ptr->name) - 1] = '\0';

    printf("Nhập GPA: ");
    scanf("%f", &ptr->gpa);

    printf("Thông tin: ");
    printf("ID= %d, ", ptr->id);
    printf("Tên= %s, ", ptr->name);
    printf("GPA= %f\n", ptr->gpa);
    printf("Địa chỉ: %p\n",(void *)ptr);


    return 0;
}