// Typedef - bí danh cho struct



#include <stdio.h>
#include <string.h>
typedef struct {
    int id;
    char name[50];
    float salary;
} Employee;

int main(void)
{
    Employee e1;

    printf("Nhập ID: ");
    scanf("%d", &e1.id);
    while (getchar() != '\n');

    printf("Nhập tên: ");
    fgets(e1.name, sizeof(e1.name), stdin);
    e1.name[strlen(e1.name) - 1] = '\0';

    printf("Nhập lương: ");
    scanf("%f", &e1.salary);

    printf("Thông tin: ");
    printf("ID=%d, Tên=%s, Lương=%.2f\n", e1.id, e1.name, e1.salary);

    return 0;
}