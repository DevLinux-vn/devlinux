#include <stdio.h>

int main(){

    float diem_so = 0.0f;

    printf("Nhập điểm số của bạn (0 - 100): ");
    if(scanf("%f", &diem_so) != 1 || (diem_so > 100 || diem_so < 0)){
        printf("Nhập lỗi!\n");
        return 1;
    }

    if(diem_so >= 90){
        printf("Xếp loại: xuất sắc\n");
    }
    else if(diem_so < 90 && diem_so >= 80){
        printf("Xếp loại: giỏi\n");
    }
    else if(diem_so < 80 && diem_so >= 65){
        printf("Xếp loại: khá\n");
    }
    else if(diem_so < 65 && diem_so >= 50){
        printf("Xếp loại: trung bình\n");
    }
    else{
        printf("Xếp loại: yếu\n");
    }

    return 0;
}