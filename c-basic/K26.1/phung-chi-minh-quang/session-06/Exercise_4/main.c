// Sắp xếp mảng tăng dần
#include <stdio.h>

void selection_sort(int *arr, int array_size){
    for (int i = 0; i < array_size - 1; i++)
    {
        int min_idx = i;
        for (int j = i + 1; j < array_size; j++)
        {
            if (arr[j] < arr[min_idx])
            {
                min_idx = j;
            }
        }

        // swap phần tử nhỏ nhất với phần đầu tiên của mảng đang duyệt
        if (min_idx != i) // chỉ swap nếu thấy phần tử nhỏ hơn ở phía sau
        { 
            int tmp = arr[i];
            arr[i] = arr[min_idx];
            arr[min_idx] = tmp;
        }
    }
}


int main(void)
{
    int n;

    printf("Nhập số lượng phần tử: ");
    scanf("%d", &n);

    // Khởi tạo array sau khi nhập
    int n_array[n];
    int element; // lưu giá trị mỗi phần tử của array khi nhập

    printf("Nhập %d phần tử: ", n);
    for (int i = 0; i < n; i++)
    {
        scanf("%d", &element);
        n_array[i] = element;
    }

    printf("Mảng ban đầu: ");
    for (int i = 0; i < n; i++)
    {
        printf("%d ", n_array[i]);
    }

    // Begin Selection Sort
    selection_sort(n_array, n);

    printf("\n");
    printf("Mảng sau sắp xếp: ");
    for (int i = 0; i < n; i++)
    {
        printf("%d ", n_array[i]);
    }
    printf("\n");

    return 0;
}