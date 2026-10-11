// Cấp phát bộ nhớ cho chuỗi ký tự

// Viết chương trình C:
// 1. Nhập độ dài chuỗi (n)
// 2. Cấp phát bộ nhớ cho chuỗi
// 3. Nhập chuỗi
// 4. In chuỗi
// 5. Tính độ dài chuỗi
// 6. Giải phóng bộ nhớ

// **Ví dụ:**
// ```
// Nhập độ dài: 10
// Nhập chuỗi: Hello
// Chuỗi: Hello
// Độ dài: 5
// ```

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void){
    int n;
    printf("Nhập độ dài: ");
    if(scanf("%d", &n) != 1){
        printf("Lỗi: Dữ liệu nhập sai\n");
        return 1;
    }
    if(n <= 0){
        printf("Lỗi: Vui lòng nhập số nguyên dương\n");
        return 1;
    }
    while(getchar() != '\n');

    char *s = malloc(n * sizeof(char));
    if(s == NULL){
        printf("Lỗi: Không đủ bộ nhớ RAM\n");
        return 1;
    }

    printf("Nhập chuỗi: ");
    fgets(s, n, stdin);
    int len = strlen(s);
    if(len > 0 && s[len - 1] == '\n'){
        s[len - 1] = '\0';
        len = len - 1;
    }
    
    printf("Chuỗi: %s\n", s);
    printf("Độ dài: %d\n", len);

    //Giải phóng bộ nhớ
    free(s);
    return 0;
}