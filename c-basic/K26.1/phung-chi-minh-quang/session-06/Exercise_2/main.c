//Tìm phần tử lớn nhất, nhỏ nhất
#include <stdio.h>

int main(void){
    int n;

    printf("Nhập số lượng phần tử: ");
    if (scanf("%d", &n) != 1)
    {
        printf("Lỗi: Dữ liệu nhập sai\n");
        return 1;
    }
    if(n<=0){
        printf("Lỗi: Vui lòng nhập số nguyên dương\n");
        return 1;
    }

    int elements[n];
    int max_pos = 0;
    int min_pos = 0;
    printf("Nhập %d phần tử: ", n);
    for (int i = 0; i < n; i++)
    {
        if (scanf("%d", &elements[i]) != 1)
        {
            printf("Lỗi: Dữ liệu nhập sai\n");
            return 1;
        }
        if (elements[i] > elements[max_pos])
        {
            max_pos = i;
        }
        if (elements[i] < elements[min_pos])
        {
            min_pos = i;
        }
    }
    //In mảng ban đầu
    printf("Mảng ban đầu: ");
    for (int i = 0; i < n; i++)
    {
        printf("%d ", elements[i]);
    }
    printf("\n");
    
    printf("Phần tử lớn nhất: %d (vị trí %d)\n", elements[max_pos], max_pos);
    printf("Phần tử nhỏ nhất: %d (vị trí %d)\n", elements[min_pos], min_pos);
    return 0;
}