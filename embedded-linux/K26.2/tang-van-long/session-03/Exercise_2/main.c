#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stddef.h>

typedef struct {
    int    id;
    char   name[64];
    int    quantity;
    double price;
} Product;

void add_product(void);
void show_product(void);
void update_quantity(void);
void list_products(void);

int main(void)
{
    int choice;

    while (1)
    {
        printf("\n====== Product Management System ========\n");
        printf("1. Add product\n");
        printf("2. Show product by index\n");
        printf("3. Update quantity by index\n");
        printf("4. List all products\n");
        printf("5. Exit\n");
        printf("========================================\n");
        printf("Choose: ");

        scanf("%d", &choice);

        switch (choice)
        {
            case 1:
                add_product();
                break;

            case 2:
                show_product();
                break;

            case 3:
                update_quantity();
                break;

            case 4:
                list_products();
                break;

            case 5:
                return 0;

            default:
                printf("Invalid choice.\n");
        }
    }
}

void add_product(void)
{
    int fd;
    Product product;

    fd = open("products.dat", O_WRONLY | O_CREAT | O_APPEND, 0644);

    if (fd == -1) {
        perror("open");
        return;
    }

    printf("Enter ID: ");
    scanf("%d", &product.id);

    printf("Enter name: ");
    getchar();
    fgets(product.name, sizeof(product.name), stdin);
    product.name[strcspn(product.name, "\n")] = '\0';

    printf("Enter quantity: ");
    scanf("%d", &product.quantity);

    printf("Enter price: ");
    scanf("%lf", &product.price);

    ssize_t bytes = write(fd, &product, sizeof(Product));

    if (bytes != sizeof(Product)) {
        perror("write");
        close(fd);
        return;
    }

    printf("Product added successfully.\n");

    close(fd);
}

void show_product(void)
{
    int fd;
    int index;
    Product product;

    printf("Enter product index: ");
    scanf("%d", &index);

    if (index < 0) {
        printf("Invalid index.\n");
        return;
    }

    fd = open("products.dat", O_RDONLY);

    if (fd == -1) {
        perror("open");
        return;
    }

    off_t offset = (off_t)index * sizeof(Product);

    if (lseek(fd, offset, SEEK_SET) == (off_t)-1) {
        perror("lseek");
        close(fd);
        return;
    }

    if (read(fd, &product, sizeof(Product)) != sizeof(Product)) {
        printf("Product not found.\n");
        close(fd);
        return;
    }

    printf("ID: %d\n", product.id);
    printf("Name: %s\n", product.name);
    printf("Quantity: %d\n", product.quantity);
    printf("Price: %.2f\n", product.price);

    close(fd);
}

void update_quantity(void)
{
    int fd;
    int index;
    int new_quantity;

    printf("Enter product index: ");
    scanf("%d", &index);

    if (index < 0) {
        printf("Invalid index.\n");
        return;
    }

    printf("Enter new quantity: ");
    scanf("%d", &new_quantity);

    fd = open("products.dat", O_RDWR);

    if (fd == -1) {
        perror("open");
        return;
    }

    off_t offset = (off_t)index * sizeof(Product);

    off_t field_offset =
        offset + offsetof(Product, quantity);

    if (lseek(fd, field_offset, SEEK_SET) == (off_t)-1) {
        perror("lseek");
        close(fd);
        return;
    }

    if (write(fd, &new_quantity, sizeof(new_quantity))
            != sizeof(new_quantity)) {
        perror("write");
        close(fd);
        return;
    }

    printf("Quantity updated successfully.\n");

    close(fd);
}

void list_products(void)
{
    int fd;
    Product product;

    fd = open("products.dat", O_RDONLY);

    if (fd == -1) {
        perror("open");
        return;
    }

    while (read(fd, &product, sizeof(Product)) == sizeof(Product))
    {
        printf("ID: %d\n", product.id);
        printf("Name: %s\n", product.name);
        printf("Quantity: %d\n", product.quantity);
        printf("Price: %.2f\n", product.price);
        printf("--------------------\n");
    }

    close(fd);
}