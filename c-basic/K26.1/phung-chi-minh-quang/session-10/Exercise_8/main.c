// Kiểm tra lỗi `malloc()` và tránh rò rỉ bộ nhớ

// Viết chương trình C:
// 1. Cấp phát bộ nhớ nhiều lần
// 2. **Luôn kiểm tra** xem `malloc()` có thành công không
// 3. Nếu thất bại → in lỗi và thoát
// 4. Nếu thành công → sử dụng bộ nhớ
// 5. **Đảm bảo** mỗi `malloc()` có tương ứng `free()`
// 6. Dùng phạm vi (scope) để tránh rò rỉ bộ nhớ

// **Ví dụ:**
// ```
// Cấp phát 100MB...
// Thành công! Đã sử dụng.
// Giải phóng...
// Hoàn tất.
// ```

#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int *ptr = malloc(100 * 1024 * 1024);
    if (ptr == NULL)
    {
        printf("Lỗi: Không đủ bộ nhớ RAM\n");
        return 1;
    }
    printf("Cấp phát 100MB...\n");

    printf("Thành công! Đã sử dụng.\n");
    free(ptr);
    ptr = NULL;
    printf("Giải phóng...\n");
    return 0;
}