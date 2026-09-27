#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stddef.h>
#include <sys/types.h>

typedef struct {
    int    id;
    char   name[64];
    int    quantity;
    double price;
} Product;


/* In thông tin một sản phẩm */
void print_product(const Product *p)
{
    printf("ID       : %d\n", p->id);
    printf("Name     : %s\n", p->name);
    printf("Quantity : %d\n", p->quantity);
    printf("Price    : %.2lf\n", p->price);
}


/* 1. Thêm sản phẩm vào cuối file */
void add_product(int fd)
{
    Product p;

    printf("ID: ");
    scanf("%d", &p.id);

    printf("Name: ");
    scanf(" %63[^\n]", p.name);

    printf("Quantity: ");
    scanf("%d", &p.quantity);

    printf("Price: ");
    scanf("%lf", &p.price);

    /* Nhảy xuống cuối file */
    if (lseek(fd, 0, SEEK_END) == -1) {
        perror("lseek");
        return;
    }

    /* Ghi một Product */
    if (write(fd, &p, sizeof(Product)) != sizeof(Product)) {
        perror("write");
        return;
    }

    printf("Product added successfully.\n");
}


/* 2. Hiển thị sản phẩm theo index */
void show_product_by_index(int fd)
{
    Product p;
    int index;

    printf("Enter index: ");
    scanf("%d", &index);

    if (index < 0) {
        printf("Invalid index!\n");
        return;
    }

    /* Tính vị trí byte của Product */
    off_t offset = (off_t)index * sizeof(Product);

    /* Nhảy trực tiếp tới Product cần đọc */
    if (lseek(fd, offset, SEEK_SET) == -1) {
        perror("lseek");
        return;
    }

    /* Chỉ đọc đúng một Product */
    ssize_t bytes = read(fd, &p, sizeof(Product));

    if (bytes == sizeof(Product)) {
        print_product(&p);
    }
    else {
        printf("Product not found.\n");
    }
}


/* 3. Cập nhật quantity theo index */
void update_quantity_by_index(int fd)
{
    int index;
    int new_quantity;

    printf("Enter index: ");
    scanf("%d", &index);

    if (index < 0) {
        printf("Invalid index!\n");
        return;
    }

    /* Offset đầu record */
    off_t offset = (off_t)index * sizeof(Product);

    /* Kiểm tra record có tồn tại không */
    off_t file_size = lseek(fd, 0, SEEK_END);

    if (file_size == -1) {
        perror("lseek");
        return;
    }

    if (offset + sizeof(Product) > file_size) {
        printf("Product not found.\n");
        return;
    }

    printf("Enter new quantity: ");
    scanf("%d", &new_quantity);

    /*
     * Nhảy đúng tới field quantity:
     *
     * đầu Product
     *      +
     * vị trí quantity trong struct
     */
    off_t field_offset =
        offset + offsetof(Product, quantity);

    if (lseek(fd, field_offset, SEEK_SET) == -1) {
        perror("lseek");
        return;
    }

    /* Chỉ ghi field quantity, KHÔNG ghi lại cả Product */
    if (write(fd, &new_quantity, sizeof(int)) != sizeof(int)) {
        perror("write");
        return;
    }

    printf("Quantity updated successfully.\n");
}


/* 4. In tất cả sản phẩm */
void list_all_products(int fd)
{
    Product p;
    int index = 0;

    /* Quay về đầu file */
    if (lseek(fd, 0, SEEK_SET) == -1) {
        perror("lseek");
        return;
    }

    while (read(fd, &p, sizeof(Product)) == sizeof(Product)) {

        printf("\n--- Product [%d] ---\n", index);

        print_product(&p);

        index++;
    }

    if (index == 0) {
        printf("No products found.\n");
    }
}


int main(void)
{
    int fd;
    int choice;

    /* Mở hoặc tạo products.dat */
    fd = open("products.dat",
              O_RDWR | O_CREAT,
              0644);

    if (fd == -1) {
        perror("open");
        return 1;
    }

    while (1) {

        printf("\n===== PRODUCT MENU =====\n");
        printf("1. Add product\n");
        printf("2. Show product by index\n");
        printf("3. Update quantity by index\n");
        printf("4. List all products\n");
        printf("5. Exit\n");
        printf("Choose: ");

        scanf("%d", &choice);

        switch (choice) {

            case 1:
                add_product(fd);
                break;

            case 2:
                show_product_by_index(fd);
                break;

            case 3:
                update_quantity_by_index(fd);
                break;

            case 4:
                list_all_products(fd);
                break;

            case 5:
                close(fd);
                printf("Exit program.\n");
                return 0;

            default:
                printf("Invalid choice!\n");
        }
    }
}