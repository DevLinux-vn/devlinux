#include <stdio.h>

int main() {
    int money, spent = 0;
    int choice;

    // 1. Nhập số tiền ban đầu
    printf("Enter initial money: ");
    scanf("%d", &money);

    // 2. In Menu
    printf("====== MENU ======\n");
    printf("1. Mineral water (10000)\n");
    printf("2. Soft drink (15000)\n");
    printf("3. Coffee (20000)\n");
    printf("4. Milk tea (25000)\n");
    printf("5. Exit\n");

    // 3. Vòng lặp mua hàng
    do {
        printf("Choose (1-5): ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                if (money >= 10000) {
                    money = money - 10000;
                    spent = spent + 10000;
                    printf("Bought Mineral water. Money left: %d\n", money);
                } else {
                    printf("Not enough money (need 10000, have %d)!\n", money);
                }
                break;
            case 2:
                if (money >= 15000) {
                    money = money - 15000;
                    spent = spent + 15000;
                    printf("Bought Soft drink. Money left: %d\n", money);
                } else {
                    printf("Not enough money (need 15000, have %d)!\n", money);
                }
                break;
            case 3:
                if (money >= 20000) {
                    money = money - 20000;
                    spent = spent + 20000;
                    printf("Bought Coffee. Money left: %d\n", money);
                } else {
                    printf("Not enough money (need 20000, have %d)!\n", money);
                }
                break;
            case 4:
                if (money >= 25000) {
                    money = money - 25000;
                    spent = spent + 25000;
                    printf("Bought Milk tea. Money left: %d\n", money);
                } else {
                    printf("Not enough money (need 25000, have %d)!\n", money);
                }
                break;
            case 5:
                // Thoát chương trình
                printf("Exiting. Total spent: %d\n", spent);
                break;
            default:
                printf("Invalid choice!\n");
                break;
        }
    } while (choice != 5);

    return 0;
}