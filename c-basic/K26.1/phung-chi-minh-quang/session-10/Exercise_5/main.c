// Con trỏ đến con trỏ (double pointer)

// Viết chương trình C:
// 1. Khai báo con trỏ `ptr` (single pointer)
// 2. Khai báo con trỏ `pptr` trỏ đến `ptr` (double pointer)
// 3. Cấp phát bộ nhớ cho `ptr`
// 4. Thay đổi giá trị qua `pptr`
// 5. In giá trị và các địa chỉ

// **Ví dụ:**
// ```
// Giá trị: 10
// Địa chỉ của biến: 0x...
// Địa chỉ của ptr: 0x...
// Địa chỉ của pptr: 0x...
// ```

#include <stdio.h>
#include <stdlib.h>

int main(void){
    int *ptr = malloc(sizeof(int));
    if(ptr == NULL){
        printf("Lỗi: Không đủ bộ nhớ RAM\n");
        return 1;
    }
    *ptr = 10; //gán biến cho ptr
    printf("Giá trị: %d\n", *ptr);
    printf("Địa chỉ của biến: %p\n", (void *)ptr);
    printf("Địa chỉ lưu ptr: %p\n", (void *)&ptr);

    int **pptr = &ptr;
    printf("Địa chỉ của ptr lưu trong pptr: %p\n", (void *)pptr);

    //Thay đổi giá trị biến thông qua pptr
    **pptr = 20;
    printf("Giá trị sau thay đổi: %d\n", *ptr);



}