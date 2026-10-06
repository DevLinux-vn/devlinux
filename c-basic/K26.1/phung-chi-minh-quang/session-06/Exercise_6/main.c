// Xóa phần tử khỏi mảng
#include <stdio.h>

int main(void){
    int n;
    int target_el;

    printf("Nhập số lượng phần tử: ");
    if (scanf("%d", &n) != 1)
    {
        printf("Lỗi nhập liệu!\n");
        return 1;
    }
    if(n <= 0){
        printf("Lỗi: Vui lòng nhập số nguyên dương\n");
        return 1;
    }

    //Khởi tạo array sau khi nhập
    int n_array[n];

    printf("Nhập %d phần tử: ",n);
    for(int i = 0; i<n; i++){
        if (scanf("%d", &n_array[i]) != 1)
        {
            printf("Lỗi nhập liệu!\n");
            return 1;
        }
    }

    printf("Nhập số cần xóa: ");
    if (scanf("%d", &target_el) != 1)
    {
        printf("Lỗi nhập liệu!\n");
        return 1;
    }

    printf("Mảng ban đầu: ");
    for (int i = 0; i < n; i++)
    {
        printf("%d ",n_array[i]);
    }    

    //Dùng kỹ thuật 2 con trỏ để dịch phần từ trong mảng
    int reader_idx = 0;
    int writter_idx = 0;

    while(reader_idx<n){
        if(n_array[reader_idx]  != target_el){
            n_array[writter_idx] = n_array[reader_idx];
            writter_idx++;
        }
        reader_idx++;
    }
    
    int new_array_size = writter_idx;
    printf("\nMảng sau xóa: ");
    for (int i = 0; i < new_array_size; i++){
        printf("%d ",n_array[i]);
    }
    printf("\nSố phần tử còn lại: %d\n",new_array_size);

    return 0;
}
