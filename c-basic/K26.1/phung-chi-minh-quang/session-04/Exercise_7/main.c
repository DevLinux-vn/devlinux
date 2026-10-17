//Máy rút tiền ATM
#include <stdio.h>

#define NOTE_500K 500000
#define NOTE_200K 200000
#define NOTE_100K 100000
#define NOTE_50K 50000

int main(void)
{
    int money;
    int count_500k, count_200k, count_100k, count_50k;
    int total_note;

    while (1)
    {
        printf("Nhập số tiền cần rút (phải là bội số của 50k):");
        if (scanf("%d", &money) != 1)
        {
            printf("Lỗi: Dữ liệu nhập sai\n");
            return 1;
        }

        if (money % 50000 != 0 || money <= 0)
        {
            printf("Số tiền phải lớn hơn 0 và phải chia hết cho 50000\n");
            continue;
            ;
        }
        break;
    };

    count_500k = money / NOTE_500K;
    money = money % NOTE_500K;

    count_200k = money / NOTE_200K;
    money = money % NOTE_200K;

    count_100k = money / NOTE_100K;
    money = money % NOTE_100K;

    count_50k = money / NOTE_50K;
    money = money % NOTE_50K;

    total_note = count_500k + count_200k + count_100k + count_50k;

    if (total_note > 20)
    {
        printf("Quá giới hạn số tờ (Tối đa 20 tờ, bạn cần %d tờ)!\n", total_note);
    }
    else
    {
        printf("Tờ 500k: %d tờ\n", count_500k);
        printf("Tờ 200k: %d tờ\n", count_200k);
        printf("Tờ 100k: %d tờ\n", count_100k);
        printf("Tờ 50k: %d tờ\n", count_50k);
        printf("Tổng: %d tờ\n", total_note);
    }

    return 0;
}