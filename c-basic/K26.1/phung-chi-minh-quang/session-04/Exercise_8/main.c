//Đoán số bí mật
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MIN 1
#define MAX 100
int main(void)
{
    // Khởi tạo mầm ngẫu nhiên dựa trên thời gian thực
    srand(time(NULL));

    char is_play = 'y'; // y = play, n = stop play

    do
    {
        int rand_number = rand() % (MAX - MIN + 1) + MIN;
        int count = 0;
        int doan;

        printf("Số bí mật từ %d-%d. Hãy đoán:\n", MIN, MAX);

        while (1)
        {
            printf("Đoán: ");
            if (scanf("%d", &doan) != 1)
            {
                while (getchar() != '\n')
                    ;
                printf("Vui lòng nhập số hợp lệ!\n");
                continue;
            };
            count++;

            if (rand_number == doan)
            {
                printf("Chúc mừng, đoán đúng sau %d lần!\n", count);
                break;
            }
            else if (rand_number < doan)
            {
                printf("Nhỏ hơn!\n");
            }
            else
            {
                printf("Lớn hơn!\n");
            }
        }
        printf("Chơi tiếp? (y/n): ");
        scanf(" %c", &is_play);

    } while (is_play == 'y' || is_play == 'Y');

    return 0;
}