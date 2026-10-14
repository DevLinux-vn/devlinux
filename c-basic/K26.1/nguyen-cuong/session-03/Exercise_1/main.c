#include <stdio.h>

int main(){

    int a = 0; // Khai báo biến

    printf("Nhập số cần kiểm tra: ");
    if(scanf("%d", &a) != 1){ // Công thức kiểm tra số được nhập vào bằng cách scanf trả về giá trị là 1 nếu nhập đúng
        return 1;
    }

    if(a % 2 == 0){ // điều kiện xét phải số chẵn lẻ hay không
        printf("Số %d là số chẵn\n", a);
    }
    else{
        printf("Số %d là số lẻ\n", a)
    }

    return 0;   
}