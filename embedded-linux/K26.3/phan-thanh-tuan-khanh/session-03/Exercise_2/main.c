#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stddef.h>

#define FILE_NAME "products.dat"

typedef struct {
    int id;
    char name[64];
    int quantity;
    double price;
} Product;

void addProduct(int fd)
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

    lseek(fd, 0, SEEK_END);

    if (write(fd, &p, sizeof(Product)) != sizeof(Product))
    {
        perror("write");
        return;
    }

    printf("Product added.\n");
}

void showProductByIndex(int fd)
{
    int index;
    Product p;

    printf("Index: ");
    scanf("%d", &index);

    off_t offset = (off_t)index * sizeof(Product);

    if (lseek(fd, offset, SEEK_SET) == -1)
    {
        perror("lseek");
        return;
    }

    if (read(fd, &p, sizeof(Product)) != sizeof(Product))
    {
        printf("Invalid index.\n");
        return;
    }

    printf("\nProduct:\n");
    printf("ID: %d\n", p.id);
    printf("Name: %s\n", p.name);
    printf("Quantity: %d\n", p.quantity);
    printf("Price: %.2lf\n", p.price);
}

void updateQuantityByIndex(int fd)
{
    int index;
    int quantity;

    printf("Index: ");
    scanf("%d", &index);

    printf("New quantity: ");
    scanf("%d", &quantity);

    off_t record_offset =
        (off_t)index * sizeof(Product);

    off_t quantity_offset =
        record_offset + offsetof(Product, quantity);

    if (lseek(fd, quantity_offset, SEEK_SET) == -1)
    {
        perror("lseek");
        return;
    }

    if (write(fd, &quantity, sizeof(quantity))
        != sizeof(quantity))
    {
        perror("write");
        return;
    }

    printf("Quantity updated.\n");
}

void listProducts(int fd)
{
    Product p;

    lseek(fd, 0, SEEK_SET);

    printf("\n===== PRODUCT LIST =====\n");

    while (read(fd, &p, sizeof(Product))
           == sizeof(Product))
    {
        printf("ID       : %d\n", p.id);
        printf("Name     : %s\n", p.name);
        printf("Quantity : %d\n", p.quantity);
        printf("Price    : %.2lf\n", p.price);
        printf("------------------------\n");
    }
}

int main(void)
{
    int choice;

    int fd = open(FILE_NAME,
                  O_RDWR | O_CREAT,
                  0644);

    if (fd < 0)
    {
        perror("open");
        return 1;
    }

    while (1)
    {
        printf("\n===== MENU =====\n");
        printf("1. Add product\n");
        printf("2. Show product by index\n");
        printf("3. Update quantity by index\n");
        printf("4. List all products\n");
        printf("5. Exit\n");
        printf("Choice: ");

        scanf("%d", &choice);

        switch (choice)
        {
            case 1:
                addProduct(fd);
                break;

            case 2:
                showProductByIndex(fd);
                break;

            case 3:
                updateQuantityByIndex(fd);
                break;

            case 4:
                listProducts(fd);
                break;

            case 5:
                close(fd);
                return 0;

            default:
                printf("Invalid choice.\n");
        }
    }

    close(fd);
    return 0;
}