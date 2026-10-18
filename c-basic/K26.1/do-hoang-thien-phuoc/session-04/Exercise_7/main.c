#include <stdio.h>

int main() {
    int amount;
    int t500 = 0, t200 = 0, t100 = 0, t50 = 0, total_bills = 0;

    // 1. Nhập số tiền cần rút
    printf("Enter withdrawal amount: ");
    scanf("%d", &amount);

    // Kiểm tra xem số tiền có phải là bội số của 50k không
    if (amount % 50000 != 0) {
        printf("Error: Amount must be a multiple of 50000!\n");
        return 1; // Kết thúc chương trình sớm nếu nhập sai
    }

    // 2. Tính số tờ từng loại (ưu tiên tờ lớn trước)
    t500 = amount / 500000;         // Lấy số tờ 500k
    amount = amount % 500000;       // Số tiền còn lại

    t200 = amount / 200000;         // Lấy số tờ 200k
    amount = amount % 200000;       // Số tiền còn lại

    t100 = amount / 100000;         // Lấy số tờ 100k
    amount = amount % 100000;       // Số tiền còn lại

    t50 = amount / 50000;           // Lấy số tờ 50k
    amount = amount % 50000;        // Số tiền còn lại (lúc này sẽ bằng 0)

    // Tính tổng số tờ
    total_bills = t500 + t200 + t100 + t50;

    // 3. Kiểm tra giới hạn số tờ và in kết quả
    if (total_bills > 20) {
        // Vượt quá 20 tờ
        printf("Limit exceeded (> 20 bills)!\n");
    } else {
        // In chi tiết số tờ nếu hợp lệ
        printf("500k bills: %d\n", t500);
        printf("200k bills: %d\n", t200);
        printf("100k bills: %d\n", t100);
        printf(" 50k bills: %d\n", t50);
        printf("Total: %d bills\n", total_bills);
    }

    return 0;
}