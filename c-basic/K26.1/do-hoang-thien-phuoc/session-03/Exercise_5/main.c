#include <stdio.h>



int main() {
    int so;

  
    printf("Nhap vao mot so (1-7): ");
    scanf("%d", &so);

    
    switch (so) {
        case 1:
            printf("Thu 2\n");
            break;
        case 2:
            printf("Thu 3\n");
            break;
        case 3:
            printf("Thu 4\n");
            break;
        case 4:
            printf("Thu 5\n");
            break;
        case 5:
            printf("Thu 6\n");
            break;
        case 6:
            printf("Thu 7\n");
            break;
        case 7:
            printf("Chu nhat\n");
            break;
        default:
           
            printf("Loi: So nhap vao khong hop le!\n");
            break;
    }

    return 0;
}
    
    
