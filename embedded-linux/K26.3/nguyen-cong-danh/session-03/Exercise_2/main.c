#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stddef.h>
#include <errno.h>

#define FILE_NAME "products.dat"

typedef struct
{
    int id;
    char name[64];
    int quantity;
    double price;
} Product;

void add_product(int fd)
{
    Product p;

    printf("Enter ID: ");
    if (scanf("%d", &p.id) != 1)
    {
        fprintf(stderr, "Invalid ID input\n");
        return;
    }

    printf("Enter Name: ");
    if (scanf(" %63[^\n]", p.name) != 1)
    {
        fprintf(stderr, "Invalid Name input\n");
        return;
    }

    printf("Enter Quantity: ");
    if (scanf("%d", &p.quantity) != 1)
    {
        fprintf(stderr, "Invalid Quantity input\n");
        return;
    }

    printf("Enter Price: ");
    if (scanf("%lf", &p.price) != 1)
    {
        fprintf(stderr, "Invalid Price input\n");
        return;
    }

    if (lseek(fd, 0, SEEK_END) == -1)
    {
        perror("lseek");
        return;
    }

    ssize_t written = 0;
    char *buf = (char *)&p;

    while (written < (ssize_t)sizeof(Product))
    {
        ssize_t n = write(fd,
                          buf + written,
                          sizeof(Product) - written);

        if (n < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }

            perror("write");
            return;
        }

        written += n;
    }

    printf("Product added successfully.\n");
}

void show_product_by_index(int fd)
{
    int index;
    Product p;

    printf("Enter index: ");

    if (scanf("%d", &index) != 1)
    {
        fprintf(stderr, "Invalid index input\n");
        return;
    }

    if (index < 0)
    {
        printf("Invalid index.\n");
        return;
    }

    off_t offset = (off_t)index * sizeof(Product);

    if (lseek(fd, offset, SEEK_SET) == -1)
    {
        perror("lseek");
        return;
    }

    ssize_t n;

    while (1)
    {
        n = read(fd, &p, sizeof(Product));

        if (n < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }

            perror("read");
            return;
        }

        break;
    }

    if (n != (ssize_t)sizeof(Product))
    {
        printf("Invalid index.\n");
        return;
    }

    printf("\n===== PRODUCT =====\n");
    printf("ID       : %d\n", p.id);
    printf("Name     : %s\n", p.name);
    printf("Quantity : %d\n", p.quantity);
    printf("Price    : %.2lf\n", p.price);
}

void update_quantity(int fd)
{
    int index;
    int new_quantity;

    printf("Enter index: ");

    if (scanf("%d", &index) != 1)
    {
        fprintf(stderr, "Invalid index input\n");
        return;
    }

    printf("Enter new quantity: ");

    if (scanf("%d", &new_quantity) != 1)
    {
        fprintf(stderr, "Invalid quantity input\n");
        return;
    }

    if (index < 0)
    {
        printf("Invalid index.\n");
        return;
    }

    off_t record_offset = (off_t)index * sizeof(Product);

    off_t quantity_offset =
        record_offset + offsetof(Product, quantity);

    if (lseek(fd, quantity_offset, SEEK_SET) == -1)
    {
        perror("lseek");
        return;
    }

    ssize_t written = 0;
    char *buf = (char *)&new_quantity;

    while (written < (ssize_t)sizeof(int))
    {
        ssize_t n = write(fd,
                          buf + written,
                          sizeof(int) - written);

        if (n < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }

            perror("write");
            return;
        }

        written += n;
    }

    printf("Quantity updated successfully.\n");
}

void list_all_products(int fd)
{
    Product p;

    if (lseek(fd, 0, SEEK_SET) == -1)
    {
        perror("lseek");
        return;
    }

    printf("\n===== PRODUCT LIST =====\n");

    while (1)
    {
        ssize_t n = read(fd, &p, sizeof(Product));

        if (n < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }

            perror("read");
            return;
        }

        if (n == 0)
        {
            break;
        }

        if (n != (ssize_t)sizeof(Product))
        {
            fprintf(stderr, "Partial record detected.\n");
            break;
        }

        printf("ID       : %d\n", p.id);
        printf("Name     : %s\n", p.name);
        printf("Quantity : %d\n", p.quantity);
        printf("Price    : %.2lf\n", p.price);
        printf("-------------------------\n");
    }
}

int main(void)
{
    int choice;

    int fd = open(FILE_NAME, O_RDWR | O_CREAT, 0644);

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
        printf("Choose: ");

        if (scanf("%d", &choice) != 1)
        {
            fprintf(stderr, "Invalid menu input\n");

            int c;
            while ((c = getchar()) != '\n' && c != EOF)
            {
            }

            continue;
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
                list_all_products(fd);
                break;

            case 5:
                if (close(fd) == -1)
                {
                    perror("close");
                    return 1;
                }

                printf("Goodbye!\n");
                return 0;

            default:
                printf("Invalid choice.\n");
                break;
        }
    }

    return 0;
}