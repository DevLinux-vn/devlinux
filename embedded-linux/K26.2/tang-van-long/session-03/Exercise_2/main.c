#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stddef.h>
#include <sys/types.h>

#define NAMELEN 64

typedef struct {
    int    id;
    char   name[NAMELEN];
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

        if (scanf("%d", &choice) != 1)
        {
            fprintf(stderr, "Invalid choice\n");
            return 1;
        }

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
                break;
        }
    }
}

void add_product(void)
{
    int fd;
    Product product;
    ssize_t bytes;

    memset(&product, 0, sizeof(Product));

    printf("Enter ID: ");

    if (scanf("%d", &product.id) != 1)
    {
        fprintf(stderr, "Invalid ID\n");
        return;
    }

    printf("Enter name: ");

    /*
     * Remove '\n' left by scanf()
     */
    getchar();

    if (fgets(product.name, sizeof(product.name), stdin) == NULL)
    {
        fprintf(stderr, "Read error\n");
        return;
    }

    product.name[strcspn(product.name, "\n")] = '\0';

    printf("Enter quantity: ");

    if (scanf("%d", &product.quantity) != 1)
    {
        fprintf(stderr, "Invalid quantity\n");
        return;
    }

    printf("Enter price: ");

    if (scanf("%lf", &product.price) != 1)
    {
        fprintf(stderr, "Invalid price\n");
        return;
    }

    fd = open("products.dat",
              O_WRONLY | O_CREAT | O_APPEND,
              0644);

    if (fd == -1)
    {
        perror("open");
        return;
    }

    bytes = write(fd, &product, sizeof(Product));

    if (bytes == -1)
    {
        perror("write");

        if (close(fd) == -1)
        {
            perror("close");
        }

        return;
    }

    if (bytes != (ssize_t)sizeof(Product))
    {
        fprintf(stderr, "Incomplete write\n");

        if (close(fd) == -1)
        {
            perror("close");
        }

        return;
    }

    if (close(fd) == -1)
    {
        perror("close");
        return;
    }

    printf("Product added successfully.\n");
}

void show_product(void)
{
    int fd;
    int index;
    Product product;

    off_t offset;
    off_t file_size;

    ssize_t bytes;

    printf("Enter product index: ");

    if (scanf("%d", &index) != 1)
    {
        fprintf(stderr, "Invalid index\n");
        return;
    }

    if (index < 0)
    {
        printf("Invalid index.\n");
        return;
    }

    fd = open("products.dat", O_RDONLY);

    if (fd == -1)
    {
        perror("open");
        return;
    }

    offset = (off_t)index * (off_t)sizeof(Product);

    /*
     * Get file size.
     */
    file_size = lseek(fd, 0, SEEK_END);

    if (file_size == (off_t)-1)
    {
        perror("lseek");

        if (close(fd) == -1)
        {
            perror("close");
        }

        return;
    }

    /*
     * Check index before seeking.
     */
    if (offset < 0 ||
        offset >= file_size ||
        file_size - offset < (off_t)sizeof(Product))
    {
        printf("Product not found (index out of bounds).\n");

        if (close(fd) == -1)
        {
            perror("close");
        }

        return;
    }

    if (lseek(fd, offset, SEEK_SET) == (off_t)-1)
    {
        perror("lseek");

        if (close(fd) == -1)
        {
            perror("close");
        }

        return;
    }

    bytes = read(fd, &product, sizeof(Product));

    if (bytes == -1)
    {
        perror("read");

        if (close(fd) == -1)
        {
            perror("close");
        }

        return;
    }

    if (bytes != (ssize_t)sizeof(Product))
    {
        fprintf(stderr, "Incomplete product record\n");

        if (close(fd) == -1)
        {
            perror("close");
        }

        return;
    }

    printf("ID: %d\n", product.id);
    printf("Name: %s\n", product.name);
    printf("Quantity: %d\n", product.quantity);
    printf("Price: %.2f\n", product.price);

    if (close(fd) == -1)
    {
        perror("close");
        return;
    }
}

void update_quantity(void)
{
    int fd;
    int index;
    int new_quantity;

    off_t offset;
    off_t field_offset;
    off_t file_size;

    ssize_t bytes;

    printf("Enter product index: ");

    if (scanf("%d", &index) != 1)
    {
        fprintf(stderr, "Invalid index\n");
        return;
    }

    if (index < 0)
    {
        printf("Invalid index.\n");
        return;
    }

    printf("Enter new quantity: ");

    if (scanf("%d", &new_quantity) != 1)
    {
        fprintf(stderr, "Invalid quantity\n");
        return;
    }

    fd = open("products.dat", O_RDWR);

    if (fd == -1)
    {
        perror("open");
        return;
    }

    offset = (off_t)index * (off_t)sizeof(Product);

    /*
     * Check file size before lseek().
     */
    file_size = lseek(fd, 0, SEEK_END);

    if (file_size == (off_t)-1)
    {
        perror("lseek");

        if (close(fd) == -1)
        {
            perror("close");
        }

        return;
    }

    /*
     * Prevent seeking outside the file.
     */
    if (offset < 0 ||
        offset >= file_size ||
        file_size - offset < (off_t)sizeof(Product))
    {
        printf("Product not found (index out of bounds).\n");

        if (close(fd) == -1)
        {
            perror("close");
        }

        return;
    }

    /*
     * Find the quantity field inside Product.
     */
    field_offset = offset + offsetof(Product, quantity);

    if (lseek(fd, field_offset, SEEK_SET) == (off_t)-1)
    {
        perror("lseek");

        if (close(fd) == -1)
        {
            perror("close");
        }

        return;
    }

    bytes = write(fd, &new_quantity, sizeof(new_quantity));

    if (bytes == -1)
    {
        perror("write");

        if (close(fd) == -1)
        {
            perror("close");
        }

        return;
    }

    if (bytes != (ssize_t)sizeof(new_quantity))
    {
        fprintf(stderr, "Incomplete write\n");

        if (close(fd) == -1)
        {
            perror("close");
        }

        return;
    }

    if (close(fd) == -1)
    {
        perror("close");
        return;
    }

    printf("Quantity updated successfully.\n");
}

void list_products(void)
{
    int fd;
    int index = 0;

    Product product;
    ssize_t bytes;

    fd = open("products.dat", O_RDONLY);

    if (fd == -1)
    {
        perror("open");
        return;
    }

    while (1)
    {
        bytes = read(fd, &product, sizeof(Product));

        /*
         * EOF: no more data.
         */
        if (bytes == 0)
        {
            break;
        }

        if (bytes == -1)
        {
            perror("read");

            if (close(fd) == -1)
            {
                perror("close");
            }

            return;
        }

        /*
         * File contains incomplete record.
         */
        if (bytes != (ssize_t)sizeof(Product))
        {
            fprintf(stderr, "Incomplete product record\n");

            if (close(fd) == -1)
            {
                perror("close");
            }

            return;
        }

        printf("\nProduct index: %d\n", index);
        printf("ID: %d\n", product.id);
        printf("Name: %s\n", product.name);
        printf("Quantity: %d\n", product.quantity);
        printf("Price: %.2f\n", product.price);
        printf("--------------------\n");

        index++;
    }

    if (close(fd) == -1)
    {
        perror("close");
        return;
    }

    if (index == 0)
    {
        printf("No products found.\n");
    }
}