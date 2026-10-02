#include <stdio.h>

int main(void)
{
    int a, b;
    float x;

    printf("Nhập a: ");
    scanf("%d", &a);

    printf("Nhập b: ");
    scanf("%d", &b);

    if (a == 0 && b == 0)
    {
        printf("Vô số nghiệm\n");
    }
    else if (a == 0 && b != 0)
    {
        printf("Vô nghiệm\n");
    }
    else
    {
        x = -(float)b / a;
        printf("Nghiệm x = %.2f\n", x);
    }

    return 0;
}