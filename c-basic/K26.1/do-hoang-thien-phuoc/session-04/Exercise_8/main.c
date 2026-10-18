#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    int guess, secret_number, attempts;
    char play_again;

    // Khởi tạo mầm sinh số ngẫu nhiên (chỉ cần gọi 1 lần ở đầu chương trình)
    srand(time(NULL));

    do {
        // Máy sinh số ngẫu nhiên từ 1 đến 100
        secret_number = rand() % 100 + 1;
        attempts = 0; // Reset số lần đoán về 0 cho mỗi ván mới

        printf("\n--- NEW GAME ---\n");
        printf("I have a secret number between 1 and 100.\n");

        // Vòng lặp cho người dùng đoán
        do {
            printf("Enter your guess: ");
            scanf("%d", &guess);
            attempts++; // Tăng số lần đoán lên 1

            // Hệ thống phản hồi
            if (guess > secret_number) {
                printf("Too high!\n");
            } else if (guess < secret_number) {
                printf("Too low!\n");
            } else {
                printf("Congratulations! You guessed correctly after %d tries.\n", attempts);
            }
        } while (guess != secret_number);

        // Cho phép chơi vòng mới
        printf("\nPlay another round? (y/n): ");
        scanf(" %c", &play_again);

    } while (play_again == 'y' || play_again == 'Y');

    printf("Thanks for playing!\n");

    return 0;
}