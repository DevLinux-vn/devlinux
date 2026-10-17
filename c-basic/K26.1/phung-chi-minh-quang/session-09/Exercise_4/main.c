#include <stdio.h>
#include <string.h>

typedef struct{
    char street[50];
    char city[30];
    int zipcode;

}Address;

typedef struct{
    Address address;
    char name[50];
    int age;
}Person;

int main(void){
    Person p1;

    printf("Nhập tên: ");
    fgets(p1.name, sizeof(p1.name), stdin);
    p1.name[strlen(p1.name) - 1] = '\0';//Để loại bỏ kí tự Enter \n ở cuối chuỗi.

    printf("Nhập tuổi: ");
    scanf("%d", &p1.age);
    while(getchar()!='\n');//Lấy hết kí tự còn sót trong buffer

    printf("Nhập đường: ");
    fgets(p1.address.street, sizeof(p1.address.street), stdin);
    p1.address.street[strlen(p1.address.street) - 1] = '\0';//Để loại bỏ kí tự Enter \n ở cuối chuỗi.

    printf("Nhập thành phố: ");
    fgets(p1.address.city, sizeof(p1.address.city), stdin);
    p1.address.city[strlen(p1.address.city) - 1] = '\0';//Để loại bỏ kí tự Enter \n ở cuối chuỗi.

    printf("Nhập mã: ");
    scanf("%d", &p1.address.zipcode);

    printf("Thông tin: %s, %d tuổi, %s\n", p1.name, p1.age, p1.address.city);

    return 0;
}