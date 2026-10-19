
#include <stdio.h>
#include <stdlib.h>

typedef struct{
    char name[50];
    float gpa;
    int id;
}Student;

int main(void){
    int n;
    printf("Nhập số học sinh: ");
    if(scanf("%d", &n) != 1){
        printf("Lỗi: Dữ liệu nhập sai\n");
        return 1;
    }
    if(n <= 0){
        printf("Lỗi: Vui lòng nhập số nguyên dương\n");
        return 1;
    }

    Student *student_arr = malloc(n*sizeof(Student));
    if(student_arr == NULL){
        printf("Lỗi: Không đủ bộ nhớ RAM\n");
        return 1;
    }

    //Nhập thông tin n học sinh
    for(int i = 0; i < n; i++){
        printf("Nhập thông tin học sinh %d: ",i+1);
        //Nhập id, tên và gpa
        //%49s để chống tràn mảng name[50]
        if(scanf("%d %49s %f",&student_arr[i].id,student_arr[i].name,&student_arr[i].gpa) != 3){
            printf("Lỗi: Dữ liệu nhập sai\n");
            free(student_arr);//Giải phóng bộ nhớ trước khi thoát chương trình
            return 1;
        }
        while(getchar() != '\n');
    }

    //In danh sách student_arr
    printf("Danh sách:\n");
    for(int i = 0; i < n; i++){
        printf("%d. %s, %.2f\n",student_arr[i].id,student_arr[i].name,student_arr[i].gpa);
    }

    //Giải phóng bộ nhớ
    free(student_arr);
    return 0;
}