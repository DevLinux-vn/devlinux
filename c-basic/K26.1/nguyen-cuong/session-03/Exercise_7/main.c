#include <stdio.h>

int main(){

    float canh_a = 0.0f, canh_b = 0.0f, canh_c = 0.0f;
    printf("Nhập lần lượt các cạnh a, b, c của tam giác: ");
    if(scanf("%f %f %f", &canh_a, &canh_b, &canh_c) != 3 || canh_a <= 0 || canh_b <= 0 || canh_c <= 0){
        printf("Nhập lỗi!\n");
        return 1;
    }

    float test; // Công thức để sắp xếp các cạnh từ lớn đến bé theo thứ tự max = canh_a > canh_b > canh_c
    if(canh_a < canh_b){test = canh_a; canh_a = canh_b; canh_b = test;}
    if(canh_b < canh_c){test = canh_b; canh_b = canh_c; canh_c = test;}
    if(canh_a < canh_b){test = canh_a; canh_a = canh_b; canh_b = test;}

    if ( (canh_b + canh_c) > canh_a ){
        if(canh_a*canh_a == canh_b*canh_b + canh_c*canh_c){
            printf("Đây là tam giác vuông với cạnh huyền là %f\n", canh_a);
        }
        else if(canh_a == canh_c){
            printf("Đây là tam giác đều với cạnh là %f\n", canh_a);
        }
        else if(canh_a == canh_b || canh_b == canh_c){
            printf("Đây là tam giác cân\n");
        }
    }
    else{
        printf("Này không phải là tam giác!\n");
    }

    return 0;
}