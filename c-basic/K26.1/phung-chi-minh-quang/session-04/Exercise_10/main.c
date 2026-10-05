//
// Máy bán hàng tự động (switch-case + vòng lặp)

// Viết chương trình C với menu sản phẩm:

// ```
// ====== MENU ======
// 1. Nước suối (10k)
// 2. Nước ngọt (15k)
// 3. Cà phê (20k)
// 4. Trà sữa (25k)
// 5. Thoát
// ```
#include <stdio.h>

#define WATER 10000
#define SODA 15000
#define COFFEE 20000
#define MILK_TEA 25000

int main(void)
{
    int money;
    int spend_money = 0;
    int choice;
    int is_continue = 1;
    printf("Nhập số tiền ban đầu: ");
    if (scanf("%d", &money) != 1)
    {
        printf("Lỗi: Dữ liệu nhập sai\n");
        return 1;
    }

    printf("====== MENU ======\n");
    printf("1. Nước suối (10k)\n");
    printf("2. Nước ngọt (15k)\n");
    printf("3. Cà phê (20k)\n");
    printf("4. Trà sữa (25k)\n");
    printf("5. Thoát\n");

    while (is_continue)
    {
        printf("Chọn (1-5): ");
        if (scanf("%d", &choice) != 1)
        {
            printf("Lỗi: Dữ liệu nhập sai\n");
            return 1;
        }

        switch (choice)
        {
        case 1:
            if (money < WATER)
            {
                printf("Tiền không đủ (Cần %dk, có %dk)!\n", WATER / 1000, money / 1000);
            }
            else
            {
                money = money - WATER;
                spend_money = spend_money + WATER;
                printf("Đã mua nước suối. Tiền còn: %d\n", money);
            }
            break;
        case 2:
            if (money < SODA)
            {
                printf("Tiền không đủ (Cần %dk, có %dk)!\n", SODA / 1000, money / 1000);
            }
            else
            {
                money = money - SODA;
                spend_money = spend_money + SODA;
                printf("Đã mua nước ngọt. Tiền còn: %d\n", money);
            }
            break;
        case 3:
            if (money < COFFEE)
            {
                printf("Tiền không đủ (Cần %dk, có %dk)!\n", COFFEE / 1000, money / 1000);
            }
            else
            {
                money = money - COFFEE;
                spend_money = spend_money + COFFEE;
                printf("Đã mua cà phê. Tiền còn: %d\n", money);
            }
            break;
        case 4:
            if (money < MILK_TEA)
            {
                printf("Tiền không đủ (Cần %dk, có %dk)!\n", MILK_TEA / 1000, money / 1000);
            }
            else
            {
                money = money - MILK_TEA;
                spend_money = spend_money + MILK_TEA;
                printf("Đã mua trà sữa. Tiền còn: %d\n", money);
            }
            break;
        case 5:
            printf("Thoát. Đã dùng: %d\n", spend_money);
            is_continue = 0;
            break;
        default:
            printf("Lựa chọn không hợp lệ\n");
            break;
        }
    }

    return 0;
}