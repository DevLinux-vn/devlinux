// Truyền mảy vào hàm (menu nhỏ)

// Viết chương trình quản lý một mảy số nguyên với menu:

// ```
// ====== MENU ======
// 1. Nhập mảng
// 2. In mảng
// 3. Tính tổng
// 4. Tìm max
// 5. Đảo ngược mảng
// 6. Thoát
// ```
#include <stdio.h>

#define NHAP_MANG 1
#define IN_MANG 2
#define TINH_TONG 3
#define TIM_MAX 4
#define DAO_NGUOC_MANG 5
#define THOAT 6

#define ARRAY_MAX_LEN 100

// 1. Nhập mảng
void input(int *arr, int *n)
{
    printf("Nhập số lượng phần tử: ");
    
    if (scanf("%d", n) != 1)
    {
        printf("Lỗi: Dữ liệu nhập sai\n");
        return;
    }

    // validate n value
    if (*n > ARRAY_MAX_LEN || *n <= 0)
    {
        printf("Số lượng phần tử không hợp lệ\n");
        return;
    }

    printf("Nhập %d phần tử: ", *n);
    for (int i = 0; i < *n; i++)
    {
        scanf("%d", &arr[i]);
    }
}

// 2. In mảng
void print(int *arr, int n)
{
    for (int i = 0; i < n; i++)
    {
        printf("%d ", arr[i]);
    }
}

// 3. Tính tổng
int sum(int *arr, int n)
{
    int sum = 0;

    for (int i = 0; i < n; i++)
    {
        sum = sum + arr[i];
    }
    return sum;
}

int findMax(int *arr, int n)
{
    int max = arr[0]; // Giả sử max là phần tử đầu tiên

    for (int i = 0; i < n; i++)
    {
        if (max < arr[i])
        {
            max = arr[i];
        }
    }
    return max;
}

void swap(int *a, int *b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

void reverse(int *arr, int n)
{
    int *left = arr;
    int *right = arr + n - 1;

    while (left < right)
    {
        swap(left, right);
        left++;
        right--;
    }
}

int main(void)
{

    printf("====== MENU ======\n");
    printf("1. Nhập mảng\n");
    printf("2. In mảng\n");
    printf("3. Tính tổng\n");
    printf("4. Tìm max\n");
    printf("5. Đảo ngược mảng\n");
    printf("6. Thoát\n");

    int choose;
    int arr[ARRAY_MAX_LEN];
    int n = 0;
    do
    {
        printf("Chọn (1-6): ");
        scanf("%d", &choose);

        switch (choose)
        {
        case NHAP_MANG:
            input(arr, &n);
            break;
        case IN_MANG:
            printf("Mảng: ");
            print(arr, n);
            break;
        case TINH_TONG:
            printf("Tổng: %d", sum(arr, n));
            break;
        case TIM_MAX:
            printf("Max: %d", findMax(arr, n));
            break;
        case DAO_NGUOC_MANG:
            reverse(arr, n);
            printf("Mảng sau đảo: ");
            print(arr, n);
            break;
        case THOAT:
            printf("Thoát chương trình");
            break;
        default:
            break;
        }

        printf("\n");
    } while (choose != THOAT);

    return 0;
}