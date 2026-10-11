#include <stdio.h>
#include <string.h>

#define MAX_BOOK_NUM 3

typedef struct{
    char title[100];
    char author[50];
    float price;
    int pages;
    int id;
}Book;

// Nhập thông tin sách qua con trỏ
// Input: con trỏ đến struct Book
void input(Book *b){
    printf("Nhập tiêu đề: ");
    fgets(b->title, sizeof(b->title), stdin);
    b->title[strlen(b->title) - 1] = '\0';

    printf("Nhập tác giả: ");
    fgets(b->author, sizeof(b->author), stdin);
    b->author[strlen(b->author) - 1] = '\0';

    printf("Nhập giá: ");
    scanf("%f", &b->price);

    printf("Nhập số trang: ");
    scanf("%d", &b->pages);

    printf("Nhập ID: ");
    scanf("%d", &b->id);
    getchar();
}

// In thông tin sách
void print(Book b){
    printf("\"%s\", ", b.title);
    printf("tác giả: %s, ", b.author);
    printf("giá: %.2f, ", b.price);
    printf("trang: %d\n", b.pages);
}

// Tính giá trung bình của sách
float average(Book arr[], int n){
    float sum = 0;

    for (int i = 0; i < n; i++){
        sum += arr[i].price;
    }
    return sum/n;
}

int main(void){
    Book book_arr[MAX_BOOK_NUM];

    for (int i = 0; i<MAX_BOOK_NUM; i++){
        printf("Nhập thông tin sách %d: \n", i+1);
        input(&book_arr[i]);
    }

    for (int i = 0; i<MAX_BOOK_NUM; i++){
        printf("Sách %d: ", i+1);
        print(book_arr[i]);
    }

    float avg = average(book_arr, MAX_BOOK_NUM);
    printf("Giá trung bình: %.2f\n", avg);

    //in sách đắt nhất
    float max_price = book_arr[0].price;
    int max_index = 0;
    for (int i = 1; i<MAX_BOOK_NUM; i++){
        if(book_arr[i].price > max_price){
            max_price = book_arr[i].price;
            max_index = i;
        }
    }
    
    printf("Sách đắt nhất: \"%s\"(%d)\n", book_arr[max_index].title, (int)book_arr[max_index].price);
    return 0;
}