//Enum quản lý trạng thái
#include <stdio.h>

typedef enum{
    ACTIVE = 1,
    INACTIVE,
    SUSPENDED,
}Status;


int main(void){
    Status status;

    printf("1=ACTIVE, 2=INACTIVE, 3=SUSPENDED\n");
    printf("Chọn: ");
    if(scanf("%d", &status) != 1){
        printf("Chọn sai!\n");
        return 1;
    }

    switch(status){
        case ACTIVE:
            printf("Trạng thái: ACTIVE\n");
            break;
        case INACTIVE:
            printf("Trạng thái: INACTIVE\n");
            break;
        case SUSPENDED:
            printf("Trạng thái: SUSPENDED\n");
            break;
        default:
            printf("Chọn sai!\n");
            break;
    }

    return 0;
}