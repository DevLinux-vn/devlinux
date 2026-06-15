#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stddef.h>

#define FILE_NAME "products.dat"


typedef struct
{
    int id;
    char name[64];
    int quantity;
    double price;

} Product;



static void add_product(int fd)
{
    Product product;

    printf("Enter ID: ");
    scanf("%d", &product.id);

    getchar();

    printf("Enter name: ");
    fgets(product.name,
          sizeof(product.name),
          stdin);

    product.name[strcspn(product.name, "\n")] = '\0';


    printf("Enter quantity: ");
    scanf("%d", &product.quantity);


    printf("Enter price: ");
    scanf("%lf", &product.price);


    if (lseek(fd, 0, SEEK_END) == -1)
    {
        perror("lseek");
        return;
    }


    ssize_t bytes = write(fd,
                          &product,
                          sizeof(Product));


    if (bytes != sizeof(Product))
    {
        perror("write");
        return;
    }


    printf("Product added successfully\n");
}


static void show_product_by_index(int fd)
{
    int index;
    Product product;


    printf("Enter index: ");
    scanf("%d", &index);


    off_t offset = (off_t) index * sizeof(Product);


    if (lseek(fd, offset, SEEK_SET) == -1)
    {
        perror("lseek");
        return;
    }


    ssize_t bytes = read(fd,
                         &product,
                         sizeof(Product));


    if (bytes == 0)
    {
        printf("Product index not found\n");
        return;
    }


    if (bytes != sizeof(Product))
    {
        perror("read");
        return;
    }


    printf("\nProduct information\n");
    printf("-------------------\n");
    printf("ID       : %d\n", product.id);
    printf("Name     : %s\n", product.name);
    printf("Quantity : %d\n", product.quantity);
    printf("Price    : %.2lf\n", product.price);
}


static void update_quantity(int fd)
{
    int index;
    int new_quantity;


    printf("Enter product index: ");
    scanf("%d", &index);


    printf("Enter new quantity: ");
    scanf("%d", &new_quantity);


    off_t offset = (off_t) index * sizeof(Product)
                 + offsetof(Product, quantity);


    if (lseek(fd, offset, SEEK_SET) == -1)
    {
        perror("lseek");
        return;
    }


    ssize_t bytes = write(fd,
                          &new_quantity,
                          sizeof(int));


    if (bytes != sizeof(int))
    {
        perror("write");
        return;
    }


    printf("Quantity updated successfully\n");
}


static void list_all_products(int fd)
{
    Product product;


    if (lseek(fd, 0, SEEK_SET) == -1)
    {
        perror("lseek");
        return;
    }


    printf("\n======= PRODUCT LIST =======\n");


    while (1)
    {
        ssize_t bytes = read(fd,
                             &product,
                             sizeof(Product));


        if (bytes == 0)
        {
            break;
        }


        if (bytes != sizeof(Product))
        {
            perror("read");
            return;
        }


        printf("ID       : %d\n", product.id);
        printf("Name     : %s\n", product.name);
        printf("Quantity : %d\n", product.quantity);
        printf("Price    : %.2lf\n", product.price);

        printf("----------------------------\n");
    }
}


static void print_menu(void)
{
    printf("\n========== MENU ==========\n");
    printf("1. Add product\n");
    printf("2. Show product by index\n");
    printf("3. Update quantity by index\n");
    printf("4. List all products\n");
    printf("5. Exit\n");
    printf("==========================\n");
    printf("Choose: ");
}


int main(void)
{
    int choice;


    int fd = open(FILE_NAME,
                  O_RDWR | O_CREAT,
                  0644);


    if (fd == -1)
    {
        perror("open");
        return EXIT_FAILURE;
    }


    while (1)
    {
        print_menu();


        scanf("%d", &choice);


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
                    return EXIT_FAILURE;
                }

                printf("Exit program\n");
                return EXIT_SUCCESS;


            default:
                printf("Invalid choice\n");
                break;
        }
    }
}