#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define PRODUCT_NAME_SIZE 64

typedef struct {
    int     id;
    char    name[PRODUCT_NAME_SIZE];
    int     quantity;
    double  price;
}Product;

void display_menu(void)
{
    printf("1. Add product\n");
    printf("2. Show product by index\n");
    printf("3. Update quantity by index\n");
    printf("4. List all products\n");
    printf("5. Exit\n");
}

void add_product(void)
{
    Product product;
    printf("Enter product's id: ");
    scanf(" %d", &product.id);
    printf("Enter product's name: ");
    scanf("%s", product.name);
    printf("Enter product's quantity: ");
    scanf("%d", &product.quantity);
    printf("Enter product's price: ");
    scanf("%lf", &product.price);

    int fd = open("products.dat",  O_CREAT | O_WRONLY | O_APPEND, 0666);
    if (fd == -1) {
        printf("[ERROR] Cannot open file\n");
        return;
    }

    ssize_t bytes = write(fd, &product, sizeof(Product));
    if (bytes == -1) 
    {
        printf("[ERROR] Cannot write file\n");
    } 
    else if (bytes != sizeof(Product))
    {
        printf("[ERROR] Writting file incorrectly\n");
    }
    else
    {
        printf("[INF] Writting file successfully\n");
    }
    close(fd);
}

void show_product(void)
{
    Product product;
    int index;

    printf("Enter product's index: ");
    scanf(" %d", &index);

    int fd = open("products.dat", O_RDONLY);
    if (fd == -1) {
        printf("[ERROR] Cannot open file\n");
        return;
    }

    lseek(fd, index*sizeof(Product), SEEK_SET);
    ssize_t byte = read(fd, &product, sizeof(Product));
    if(byte > 0)
    {
        printf("=================================\n");
        printf("The product has existed\n");
        printf("Product's id: %d\n", product.id);
        printf("Product's name: %s\n", product.name);
        printf("Product's age: %d\n", product.quantity);
        printf("Product's gpa: %0.2f\n", product.price);
        printf("=================================\n");
    } 
    else if (byte == 0)
    {
        printf("Product with index %d is not exist\n", index);
    }
    else
    {
        printf("[ERROR] Cannot read file\n");
    }
    close(fd);
}

void update_quantity()
{
    Product product;
    int index;
    int quantity;

    printf("Enter product's index to update quantity: ");
    scanf(" %d", &index);

    int fd = open("products.dat", O_RDWR);
    if (fd == -1) {
        printf("[ERROR] Cannot open file\n");
        return;
    }

    lseek(fd, index*sizeof(Product), SEEK_SET);
    ssize_t byte = read(fd, &product, sizeof(Product));
    if(byte > 0)
    {
        printf("=================================\n");
        printf("The product has existed\n");
        printf("Product's id: %d\n", product.id);
        printf("Product's name: %s\n", product.name);
        printf("Product's quantity: %d\n", product.quantity);
        printf("Product's price: %0.2f\n", product.price);
        printf("=================================\n");
    } 
    else if (byte == 0)
    {
        printf("Product with index %d is not exist\n", index);
        close(fd);
        return;
    }
    else
    {
        printf("[ERROR] Cannot read file\n");
        close(fd);
        return;
    }

    printf("New quantity to update: ");
    scanf("%d", &quantity);
    lseek(fd, sizeof(product.id) + sizeof(product.name), SEEK_CUR);
    ssize_t bytes = write(fd, &quantity, sizeof(quantity));
    if (bytes == -1) {
        printf("[ERROR] Cannot write file\n");
        close(fd);
        return;
    }
    close(fd);
}

void list_products(void)
{
    Product product;

    int fd = open("products.dat", O_RDONLY);
    if (fd == -1) {
        printf("[ERROR] Cannot open file\n");
        return;
    }

    while(read(fd, &product, sizeof(Product)) > 0)
    {
        printf("=================================\n");
        printf("Product's id: %d\n", product.id);
        printf("Product's name: %s\n", product.name);
        printf("Product's quantity: %d\n", product.quantity);
        printf("Product's price: %0.2f\n", product.price);
        printf("=================================\n");
    }
    close(fd);
}

int main()
{
    char key_input;
    while(1)
    {
        display_menu();
        scanf(" %c", &key_input);
        switch(key_input)
        {
            case '1':
                add_product();
                break;
            case '2':
                show_product();
                break;
            case '3':
                update_quantity();
                break;
            case '4':
                list_products();
                break;
            case '5':
                return 0;
            default:
                break;
        }
    }
    return 0;
}