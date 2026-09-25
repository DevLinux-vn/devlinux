#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stddef.h>

typedef struct
{
    int id;
    char name[64];
    int quantity;
    double price;
} Product;

void add_product(int fd);
void show_product_by_index(int fd);
void update_quantity(int fd);
void list_products(int fd);

void add_product(int fd)
{
    Product p;

    printf("ID: ");

    if (scanf("%d", &p.id) != 1)
    {
        printf("Invalid ID\n");
        return;
    }

    if (p.id <= 0)
    {
        printf("ID must be positive\n");
        return;
    }

    printf("Name: ");
    scanf(" %63[^\n]", p.name);

    printf("Quantity: ");

    if (scanf("%d", &p.quantity) != 1)
    {
        printf("Invalid quantity\n");
        return;
    }

    if (p.quantity < 0)
    {
        printf("Quantity must be non-negative\n");
        return;
    }

    printf("Price: ");

    if (scanf("%lf", &p.price) != 1)
    {
        printf("Invalid price\n");
        return;
    }

    if (p.price < 0)
    {
        printf("Price must be non-negative\n");
        return;
    }

    if (lseek(fd, 0, SEEK_END) == (off_t)-1)
    {
        perror("lseek");
        return;
    }

    if (write(fd, &p, sizeof(Product)) != sizeof(Product))
    {
        perror("write");
        return;
    }

    printf("Product added successfully\n");

}

void list_products(int fd)
{
    Product p;
    ssize_t bytes;

    if (lseek(fd, 0, SEEK_SET) == (off_t)-1)
    {
        perror("lseek");
        return;
    }

    while ((bytes = read(fd,
                         &p,
                         sizeof(Product)))
            == sizeof(Product))
    {
        printf("\n");
        printf("ID       : %d\n", p.id);
        printf("Name     : %s\n", p.name);
        printf("Quantity : %d\n", p.quantity);
        printf("Price    : %.2f\n", p.price);
    }

    if (bytes < 0)
    {
        perror("read");
    }
}

void show_product_by_index(int fd)
{
    int index;
    Product p;

    printf("Index: ");

    if (scanf("%d", &index) != 1)
    {
        printf("Invalid index\n");
        return;
    }

    if (index < 0)
    {
        printf("Invalid index\n");
        return;
    }

    off_t filesize =
        lseek(fd, 0, SEEK_END);

    if (filesize == (off_t)-1)
    {
        perror("lseek");
        return;
    }

    int count =
        filesize / sizeof(Product);

    if (index >= count)
    {
        printf("Index out of range\n");
        return;
    }

    off_t offset =
        (off_t)index * sizeof(Product);

    if (lseek(fd,
              offset,
              SEEK_SET)
        == (off_t)-1)
    {
        perror("lseek");
        return;
    }

    if (read(fd,
             &p,
             sizeof(Product))
        != sizeof(Product))
    {
        perror("read");
        return;
    }

    printf("\n");
    printf("ID       : %d\n", p.id);
    printf("Name     : %s\n", p.name);
    printf("Quantity : %d\n", p.quantity);
    printf("Price    : %.2f\n", p.price);
}
void update_quantity(int fd)
{
    int index;
    int quantity;

    printf("Index: ");

    if (scanf("%d", &index) != 1)
    {
        printf("Invalid index\n");
        return;
    }

    if (index < 0)
    {
        printf("Invalid index\n");
        return;
    }

    printf("New quantity: ");

    if (scanf("%d", &quantity) != 1)
    {
        printf("Invalid quantity\n");
        return;
    }

    if (quantity < 0)
    {
        printf("Quantity must be non-negative\n");
        return;
    }

    off_t filesize =
        lseek(fd, 0, SEEK_END);

    if (filesize == (off_t)-1)
    {
        perror("lseek");
        return;
    }

    int count =
        filesize / sizeof(Product);

    if (index >= count)
    {
        printf("Index out of range\n");
        return;
    }

    off_t record_offset =
        (off_t)index * sizeof(Product);

    off_t field_offset =
        record_offset +
        offsetof(Product,
                 quantity);

    if (lseek(fd,
              field_offset,
              SEEK_SET)
        == (off_t)-1)
    {
        perror("lseek");
        return;
    }

    if (write(fd,
              &quantity,
              sizeof(quantity))
        != sizeof(quantity))
    {
        perror("write");
        return;
    }

    printf("Quantity updated successfully\n");
}
int main(void)
{
    int fd;
    int choice;

    fd = open("products.dat",
              O_RDWR | O_CREAT,
              0644);

    if (fd < 0)
    {
        perror("open");
        return 1;
    }

    while (1)
    {
        printf("\n");
        printf("1. Add product\n");
        printf("2. Show product by index\n");
        printf("3. Update quantity by index\n");
        printf("4. List all products\n");
        printf("5. Exit\n");
        printf("Choice: ");

        if (scanf("%d",
                  &choice)
            != 1)
        {
            printf("Invalid choice\n");
            break;
        }

        switch (choice)
        {
            case 1:
                add_product(fd);
                break;

            case 2:
                show_product_by_index(fd);
                break;

            case 3:
                update_quantity(fd);
                break;

            case 4:
                list_products(fd);
                break;

            case 5:
                close(fd);
                return 0;

            default:
                printf("Invalid choice\n");
        }
    }

    close(fd);

    return 0;
}

