//Exercise_6: Tính lương ròng sau thuế
#include <stdio.h>

int main(void){
    float salary, tax_rate, tax_amount, net_salary;
    
    printf("Nhập lương brutto: ");
    scanf("%f",&salary);

    printf("Nhập tỉ lệ thuế (%%): ");
    scanf("%f",&tax_rate);

    tax_amount = salary*tax_rate/100;
    printf("Tiền thuế: %.2f\n",tax_amount);

    net_salary = salary - tax_amount;
    printf("Lương ròng: %.2f\n",net_salary);

    return 0;
}