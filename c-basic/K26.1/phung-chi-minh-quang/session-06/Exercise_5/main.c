//Tìm kiếm tuyến tính và nhị phân
#include <stdio.h>

int linearSearch(int *arr, int array_size, int target_el)
{
    for (int i = 0; i < array_size; i++)
    {
        if (arr[i] == target_el)
        {
            return i;
            break;
        }
    }
    return -1;
}

int binarySearch(int *arr, int array_size, int target_el)
{
    int left_idx = 0;
    int right_idx = array_size - 1;

    while (left_idx <= right_idx)
    {
        int mid_idx = left_idx + (right_idx - left_idx) / 2; // The same with (right_idx + left_idx)/2;

        if (arr[mid_idx] == target_el)
        {
            return mid_idx;
        }
        else if (arr[mid_idx] < target_el)
        {
            left_idx = mid_idx + 1;
        }
        else
        {
            // arr[mid_idx] > target_el
            right_idx = mid_idx - 1;
        }
    }
    return -1;
}

int main(void)
{
    int n;
    int target_el;

    printf("Nhập số lượng phần tử: ");
    scanf("%d", &n);

    // Khởi tạo array sau khi nhập
    int n_array[n];

    printf("Nhập %d phần tử (đã sắp xếp): ", n);
    for (int i = 0; i < n; i++)
    {
        scanf("%d", &n_array[i]);
    }

    printf("Nhập số cần tìm: ");
    scanf("%d", &target_el);

    // Tìm kiếm tuyến tính
    int linear_res = linearSearch(n_array, n, target_el); // linear result
    if (linear_res != -1)
    {
        printf("Tìm kiếm tuyến tính: Tìm thấy tại vị trí %d\n", linear_res);
    }
    else
    {
        printf("Tìm kiếm tuyến tính: Không tìm thấy\n");
    }

    // Tìm kiếm nhị phân
    int binary_res = binarySearch(n_array, n, target_el); // binary result
    if (binary_res != -1)
    {
        printf("Tìm kiếm nhị phân: Tìm thấy tại vị trí %d\n", binary_res);
    }
    else
    {
        printf("Tìm kiếm nhị phân: Không tìm thấy phần tử\n");
    }

    return 0;
}