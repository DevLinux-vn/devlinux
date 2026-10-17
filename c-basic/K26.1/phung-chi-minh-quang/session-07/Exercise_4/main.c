//Tính tổng mảng (dùng con trỏ)
#include <stdio.h>

int sum(int *arr, int n){
    int sum = 0;
    for(int i = 0; i < n; i++){
        sum += arr[i];
    }
    return sum;
}

int main(void){
    int n;
    printf("Nhập số lượng phần tử: ");
    if(scanf("%d", &n) != 1){
        printf("Lỗi: Dữ liệu nhập sai\n");
        return 1;
    }
    if(n <= 0){
        printf("Lỗi: Vui lòng nhập số nguyên dương\n");
        return 1;
    }
    
    printf("Nhập %d phần tử: ", n);
    int arr[n];
    for(int i = 0; i < n; i++){
        if(scanf("%d", &arr[i]) != 1){
            printf("Lỗi: Dữ liệu nhập sai\n");
            return 1;
        }
    }

    printf("Tổng mảng: %d\n",sum(arr,n));

    return 0;
}