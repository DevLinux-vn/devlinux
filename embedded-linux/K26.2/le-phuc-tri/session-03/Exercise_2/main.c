#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stddef.h>

#define FILE_NAME "products.dat"

typedef struct {
    int    id;
    char   name[64];
    int    quantity;
    double price;
} Product;


void add_product(int fd)
{
    Product product;

    printf("\n===== Add Product =====\n");

    printf("Enter ID: ");
    scanf("%d", &product.id);

    printf("Enter name: ");
    scanf(" %63[^\n]", product.name);

    printf("Enter quantity: ");
    scanf("%d", &product.quantity);

    printf("Enter price: ");
    scanf("%lf", &product.price);

    /*
     * Move to the end of the file.
     */
    if (lseek(fd, 0, SEEK_END) == -1) {
        perror("lseek");
        return;
    }

    /*
     * Append one Product.
     */
    if (write(fd, &product, sizeof(Product)) != sizeof(Product)) {
        perror("write");
        return;
    }

    printf("Product added successfully.\n");
}


void show_product(int fd)
{
    Product product;
    int index;

    printf("\nEnter product index: ");
    scanf("%d", &index);

    if (index < 0) {
        printf("Invalid index.\n");
        return;
    }

    /*
     * Calculate byte offset of the product.
     */
    off_t offset = (off_t)index * sizeof(Product);

    /*
     * Jump directly to that product.
     */
    if (lseek(fd, offset, SEEK_SET) == -1) {
        perror("lseek");
        return;
    }

    /*
     * Read exactly one Product.
     */
    ssize_t bytes = read(fd, &product, sizeof(Product));

    if (bytes == 0) {
        printf("Product index %d does not exist.\n", index);
        return;
    }

    if (bytes != sizeof(Product)) {
        perror("read");
        return;
    }

    printf("\n===== Product =====\n");
    printf("Index    : %d\n", index);
    printf("ID       : %d\n", product.id);
    printf("Name     : %s\n", product.name);
    printf("Quantity : %d\n", product.quantity);
    printf("Price    : %.2f\n", product.price);
}


void update_quantity(int fd)
{
    int index;
    int new_quantity;

    printf("\nEnter product index: ");
    scanf("%d", &index);

    if (index < 0) {
        printf("Invalid index.\n");
        return;
    }

    printf("Enter new quantity: ");
    scanf("%d", &new_quantity);

    /*
     * Offset of the target record.
     */
    off_t offset = (off_t)index * sizeof(Product);

    /*
     * Offset of quantity field inside Product.
     */
    off_t field_offset =
        offset + offsetof(Product, quantity);

    /*
     * Jump directly to quantity.
     */
    if (lseek(fd, field_offset, SEEK_SET) == -1) {
        perror("lseek");
        return;
    }

    /*
     * Write ONLY quantity.
     */
    if (write(fd, &new_quantity, sizeof(new_quantity))
        != sizeof(new_quantity)) {
        perror("write");
        return;
    }

    printf("Quantity updated successfully.\n");
}


void list_products(int fd)
{
    Product product;
    int index = 0;

    /*
     * Start from the beginning.
     */
    if (lseek(fd, 0, SEEK_SET) == -1) {
        perror("lseek");
        return;
    }

    printf("\n========== Product List ==========\n");

    while (read(fd, &product, sizeof(Product)) == sizeof(Product)) {

        printf("\nProduct [%d]\n", index);
        printf("ID       : %d\n", product.id);
        printf("Name     : %s\n", product.name);
        printf("Quantity : %d\n", product.quantity);
        printf("Price    : %.2f\n", product.price);

        index++;
    }

    if (index == 0) {
        printf("No products found.\n");
    }
}


int main(void)
{
    int fd;
    int choice;

    /*
     * Open or create products.dat.
     */
    fd = open(FILE_NAME, O_RDWR | O_CREAT, 0644);

    if (fd == -1) {
        perror("open");
        return EXIT_FAILURE;
    }

    while (1) {

        printf("\n=================================\n");
        printf("       PRODUCT MANAGEMENT\n");
        printf("=================================\n");
        printf("1. Add product\n");
        printf("2. Show product by index\n");
        printf("3. Update quantity by index\n");
        printf("4. List all products\n");
        printf("5. Exit\n");
        printf("Choose: ");

        scanf("%d", &choice);

        switch (choice) {

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
                printf("Program terminated.\n");
                return 0;

            default:
                printf("Invalid choice.\n");
        }
    }
}
