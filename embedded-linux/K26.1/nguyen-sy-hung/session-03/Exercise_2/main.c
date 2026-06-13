#include <stdio.h>
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


void add_product(int fd)
{
    Product product;

    printf("Enter ID: ");
    scanf("%d", &product.id);

    printf("Enter name: ");
    scanf(" %63[^\n]", product.name);

    printf("Enter quantity: ");
    scanf("%d", &product.quantity);

    printf("Enter price: ");
    scanf("%lf", &product.price);


    lseek(fd, 0, SEEK_END);

    write(fd, &product, sizeof(Product));
}


void show_product(int fd)
{
    int index;
    Product product;
    off_t offset;


    printf("Enter index: ");
    scanf("%d", &index);


    offset = (off_t)index * sizeof(Product);


    lseek(fd, offset, SEEK_SET);


    if (read(fd, &product, sizeof(Product)) == sizeof(Product))
    {
        printf("ID: %d\n", product.id);
        printf("Name: %s\n", product.name);
        printf("Quantity: %d\n", product.quantity);
        printf("Price: %.2lf\n", product.price);
    }
    else
    {
        printf("Product not found\n");
    }
}


void update_quantity(int fd)
{
    int index;
    int quantity;
    off_t offset;


    printf("Enter index: ");
    scanf("%d", &index);

    printf("Enter new quantity: ");
    scanf("%d", &quantity);


    offset = (off_t)index * sizeof(Product)
             + offsetof(Product, quantity);


    lseek(fd, offset, SEEK_SET);


    write(fd, &quantity, sizeof(int));
}


void list_products(int fd)
{
    Product product;


    lseek(fd, 0, SEEK_SET);


    while (read(fd, &product, sizeof(Product)) == sizeof(Product))
    {
        printf("ID: %d\n", product.id);
        printf("Name: %s\n", product.name);
        printf("Quantity: %d\n", product.quantity);
        printf("Price: %.2lf\n", product.price);
        printf("-----------------\n");
    }
}


int main()
{
    int fd;
    int choice;


    fd = open(FILE_NAME, O_RDWR | O_CREAT, 0644);


    if (fd < 0)
    {
        printf("Cannot open file\n");
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

        scanf("%d", &choice);


        switch (choice)
        {
            case 1:
                add_product(fd);
                break;


            case 2:
                show_product(fd);
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
}