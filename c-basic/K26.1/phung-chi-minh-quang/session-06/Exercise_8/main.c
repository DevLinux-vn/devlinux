//Ma trận vuông – đường chéo
#include <stdio.h>

int main(void)
{
    int n;

    printf("Nhập cấp ma trận: ");
    scanf("%d", &n);

    int arr[n][n];

    printf("Nhập ma trận %dx%d\n", n, n);
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            scanf("%d", &arr[i][j]);
        }
    }

    printf("Ma trận:");
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            printf("%d ", arr[i][j]);
        }
        printf("\n");
    }

    // tính tổng đường chéo chính + phu
    int total_cheo_chinh = 0;
    int total_cheo_phu = 0;
    for (int i = 0; i < n; i++)
    {
        total_cheo_chinh = total_cheo_chinh + arr[i][i];
        total_cheo_phu = total_cheo_phu + arr[i][n - i - 1];
    }
    printf("Tổng đường chéo chính: %d\n", total_cheo_chinh);
    printf("Tổng đường chéo phụ: %d\n", total_cheo_phu);

    return 0;
}