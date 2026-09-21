#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct {
    int    id;
    char   name[64];
    int    quantity;
    double price;
} Product;

void Display_Menu(void)
{
    printf("1. Add product\n");
    printf("2. Show product by index\n");
    printf("3. Update quantity by index\n");
    printf("4. List all products\n");
    printf("5. Exit\n");
}

void Add_Product(void)
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

    int fd = open("products.dat",  O_CREAT | O_WRONLY, 0666);
    if (fd == -1) {
        printf("[ERROR] Cannot open file\n");
        return;
    }

    lseek(fd, 0, SEEK_END);
    ssize_t bytes = write(fd, &product, sizeof(Product));
    if (bytes == -1) {
        printf("[ERROR] Cannot write file\n");
        close(fd);
        return;
    }
    close(fd);
}

void Show_Product(void)
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
    if(read(fd, &product, sizeof(Product)) > 0)
    {
        printf("=================================\n");
        printf("The product has existed\n");
        printf("Product's id: %d\n", product.id);
        printf("Product's name: %s\n", product.name);
        printf("Product's age: %d\n", product.quantity);
        printf("Product's gpa: %0.2f\n", product.price);
        printf("=================================\n");
    } 
    else if (read(fd, &product, sizeof(Product)) == 0)
    {
        printf("Product with index %d is not exist\n", index);
    }
    else
    {
        printf("[ERROR] Cannot read file\n");
    }
    close(fd);
}

void Update_Quantity()
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
    if(read(fd, &product, sizeof(Product)) > 0)
    {
        printf("=================================\n");
        printf("The product has existed\n");
        printf("Product's id: %d\n", product.id);
        printf("Product's name: %s\n", product.name);
        printf("Product's quantity: %d\n", product.quantity);
        printf("Product's price: %0.2f\n", product.price);
        printf("=================================\n");
    } 
    else if (read(fd, &product, sizeof(Product)) == 0)
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

void List_Products(void)
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
    char KeyInput;
    while(1)
    {
        Display_Menu();
        scanf(" %c", &KeyInput);
        switch(KeyInput)
        {
            case '1':
                Add_Product();
                break;
            case '2':
                Show_Product();
                break;
            case '3':
                Update_Quantity();
                break;
            case '4':
                List_Products();
                break;
            case '5':
                return 0;
            default:
                break;
        }
    }
    return 0;
}