#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stddef.h>

#define FILE_NAME "products.dat"
#define PRODUCT_NAME_SIZE 64

typedef struct {
    int id;
    char name[PRODUCT_NAME_SIZE];
    int quantity;
    double price;
} Product;

static int write_full(int fd, const void *buf, size_t count)
{
    const char *ptr = (const char *)buf;
    size_t total = 0;

    while (total < count)
    {
        ssize_t n = write(fd,
                          ptr + total,
                          count - total);

        if (n < 0)
        {
            perror("write");
            return -1;
        }

        total += n;
    }

    return 0;
}

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

    if (lseek(fd, 0, SEEK_END) == (off_t)-1)
    {
        perror("lseek");
        return;
    }

    if (write_full(fd, &p, sizeof(Product)) == -1)
    {
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

    if (index < 0)
    {
        printf("Index must be >= 0.\n");
        return;
    }

    off_t offset = (off_t)index * sizeof(Product);

    if (lseek(fd, offset, SEEK_SET) == (off_t)-1)
    {
        perror("lseek");
        return;
    }

    ssize_t n = read(fd, &p, sizeof(Product));

    if (n == -1)
    {
        perror("read");
        return;
    }

    if (n != sizeof(Product))
    {
        printf("Invalid index.\n");
        return;
    }

    printf("\nProduct Information\n");
    printf("ID       : %d\n", p.id);
    printf("Name     : %s\n", p.name);
    printf("Quantity : %d\n", p.quantity);
    printf("Price    : %.2lf\n", p.price);
}

void updateQuantityByIndex(int fd)
{
    int index;
    int quantity;

    printf("Index: ");
    scanf("%d", &index);

    if (index < 0)
    {
        printf("Index must be >= 0.\n");
        return;
    }

    printf("New quantity: ");
    scanf("%d", &quantity);

    off_t record_offset =
        (off_t)index * sizeof(Product);

    off_t quantity_offset =
        record_offset +
        offsetof(Product, quantity);

    if (lseek(fd, quantity_offset, SEEK_SET)
        == (off_t)-1)
    {
        perror("lseek");
        return;
    }

    if (write_full(fd,
                   &quantity,
                   sizeof(quantity)) == -1)
    {
        return;
    }

    printf("Quantity updated.\n");
}

void listProducts(int fd)
{
    Product p;

    if (lseek(fd, 0, SEEK_SET) == (off_t)-1)
    {
        perror("lseek");
        return;
    }

    printf("\n===== PRODUCT LIST =====\n");

    while (1)
    {
        ssize_t n =
            read(fd, &p, sizeof(Product));

        if (n == -1)
        {
            perror("read");
            break;
        }

        if (n != sizeof(Product))
        {
            break;
        }

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
                if (close(fd) == -1)
                {
                    perror("close");
                    return 1;
                }

                return 0;

            default:
                printf("Invalid choice.\n");
        }
    }

    if (close(fd) == -1)
    {
        perror("close");
        return 1;
    }

    return 0;
}