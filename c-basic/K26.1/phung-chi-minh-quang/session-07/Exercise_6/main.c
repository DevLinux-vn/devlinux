// Đếm số nguyên tố trong mảng (dùng con trỏ)

// Viết chương trình C:
// 1. Viết hàm `int isPrime(int num)` để kiểm tra một số có phải nguyên tố không
// 2. Viết hàm `int countPrimes(int *arr, int n)` để đếm số lượng số nguyên tố trong mảng (dùng con trỏ để duyệt)
// 3. Trong `main()`:
//    - Nhập mảng `n` số nguyên
//    - Gọi hàm `countPrimes()` để đếm số nguyên tố
//    - In kết quả

// **Ví dụ:**
// ```
// Nhập số lượng phần tử: 6
// Nhập 6 phần tử: 2 3 4 5 6 7
// Số lượng số nguyên tố: 4
// ```

#include <stdio.h>

int isPrime(int num)
{
    if (num <= 1)
    { // Nếu số nguyên tố nhỏ hơn 1, thì không phải là số nguyên tố
        return 0;
    }

    for (int i = 2; i * i <= num; i++)
    {
        if (num % i == 0)
        {
            // num không phải số nguyên tố;
            return 0;
        }
    }
    return 1;
}

int countPrimes(int *arr, int n)
{
    int count = 0;
    int *p = arr;

    for (int i = 0; i < n; i++)
    {
        if (isPrime(*p))
        {
            count++;
        }
        p++;
    }

    return count;
}

int main(void)
{
    int n;
    printf("Nhập số lượng phần tử: ");
    if (scanf("%d", &n) != 1)
    {
        printf("Lỗi: Dữ liệu nhập sai\n");
        return 1;
    }
    if (n <= 0)
    {
        printf("Lỗi: Vui lòng nhập số nguyên dương\n");
        return 1;
    }

    printf("Nhập %d phần tử: ", n);
    int arr[n];
    for (int i = 0; i < n; i++)
    {
        if (scanf("%d", &arr[i]) != 1)
        {
            printf("Lỗi: Dữ liệu nhập sai\n");
            return 1;
        }
    }

    printf("Số lượng số nguyên tố: %d\n", countPrimes(arr, n));

    return 0;
}