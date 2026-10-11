#include <stdio.h>
#include <string.h>
typedef enum
{
    IT = 0,
    HR,
    SALES,
} Department;

typedef struct
{
    Department department;
    char name[50];
    int ID;
} Employee;

void printEmployee(Employee e)
{
    switch (e.department)
    {
    case IT:
        printf("ID=%d, Tên=%s , Phòng=IT\n", e.ID, e.name);
        break;
    case HR:
        printf("ID=%d, Tên=%s , Phòng=HR\n", e.ID, e.name);
        break;
    case SALES:
        printf("ID=%d, Tên=%s , Phòng=SALES\n", e.ID, e.name);
        break;
    default:
        printf("Phòng không hợp lệ!\n");
        break;
    }
}

int main(void)
{
    Employee emp_arr[2];

    for (int i = 0; i < 2; i++)
    {
        Employee emp;
        printf("Nhập thông tin nhân viên %d: \n", i + 1);
        printf("Phòng (IT=0, HR=1, SALES=2): ");
        scanf("%d", (int *)&emp.department);

        while (getchar() != '\n')
            ;
        printf("Tên: ");
        fgets(emp.name, sizeof(emp.name), stdin);
        emp.name[strlen(emp.name) - 1] = '\0';

        printf("ID: ");
        scanf("%d", &emp.ID);

        emp_arr[i] = emp;
    }

    for (int i = 0; i < 2; i++)
    {
        printf("Nhân viên %d: ", i + 1);
        printEmployee(emp_arr[i]);
    }

    return 0;
}