#include <stdio.h>
#include <stdlib.h>

int main(void){
    int m;//số dòng
    int n;//số cột
    printf("Nhập số dòng và cột: ");
    if(scanf("%d %d", &m, &n) != 2 || m <= 0 || n <= 0){
        printf("Lỗi: Vui lòng nhập số nguyên dương!\n");
        return 1;
    }

    int **matrix = malloc(m*sizeof(int*));//lưu địa chỉ của mảng matrix
    if(matrix == NULL){
        printf("Lỗi: Không đủ bộ nhớ RAM\n");
        return 1;
    }

    for(int i = 0; i < m; i++){
        matrix[i] = malloc(n*sizeof(int));//cấp phát bộ nhớ để chứa n số nguyên cho mỗi hàng
        
        if(matrix[i] == NULL){
            printf("Lỗi: Không đủ bộ nhớ RAM\n");
            for(int j = 0; j < i; j++){//Khi có lỗi thì giải phóng lần lượt từng hàng
                free(matrix[j]);
            }
            free(matrix); //Giải phóng bộ nhớ của matrix
            return 1;
        }
    }

    //Nhập ma trận
    printf("Nhập ma trận:\n");
    for(int i = 0; i < m; i++){
        for(int j = 0; j < n; j++){
            if(scanf("%d", &matrix[i][j]) != 1){
                printf("Lỗi: Dữ liệu nhập sai\n");
                for(int k = 0; k < i; k++){//Khi có lỗi thì giải phóng lần lượt từng hàng
                    free(matrix[k]);
                }
                free(matrix); //Giải phóng bộ nhớ của matrix
                return 1;
            }
        }
    }

    //In ma trận
    printf("Ma trận:\n");
    for(int i = 0; i < m; i++){
        for(int j = 0; j < n; j++){
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }

    //Giải phóng bộ nhớ
    for(int i = 0; i < m; i++){
        free(matrix[i]);
    }
    free(matrix);
    matrix = NULL;
    return 0;
}