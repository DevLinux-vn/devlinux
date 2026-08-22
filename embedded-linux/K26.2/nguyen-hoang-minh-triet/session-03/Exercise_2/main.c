#include <stdio.h>
#include <stdlib.h>
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

/*
 * @brief Add a new product to the end of the file. 
 */
static int add_product(int fd);

/*
 * @brief Show a product by its index.
 */
static int show_product(int fd);

/*
 * @brief Update the quantity of a product by its index.
 */
static int update_quantity(int fd);

/*
 * @brief List all products in the file.
 */
static int list_products(int fd);

int main(void)  {
    int fd;
    int choice;

    fd = open(FILE_NAME, O_RDWR | O_CREAT, 0644);

    if (fd == -1) {
        perror("open");
        return EXIT_FAILURE;
    }

    while (1) {
        printf("\n");
        printf("===== Product Management =====\n");
        printf("1. Add product\n");
        printf("2. Show product by index\n");
        printf("3. Update quantity by index\n");
        printf("4. List all products\n");
        printf("5. Exit\n");
        printf("Choose: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid choice.\n");

            while (getchar() != '\n') {
                /* Discard invalid input. */
            }

            continue;
        }

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
                if (close(fd) == -1)
                {
                    perror("close");
                    return EXIT_FAILURE;
                }

                printf("Goodbye.\n");
                return EXIT_SUCCESS;

            default:
                printf("Invalid choice.\n");
                break;
        }
    }
}

/*
 * @brief Add a new product to the end of the file.
 *
 * @param fd File descriptor of the opened file.
 * @return 0 on success, -1 on error.
 */
static int add_product(int fd)  {
    Product product;
    ssize_t bytes_written;

    printf("Enter ID: ");
    if (scanf("%d", &product.id) != 1)  {
        printf("Invalid ID.\n");
        return -1;
    }

    printf("Enter name: ");
    if (scanf(" %63[^\n]", product.name) != 1) {
        printf("Invalid name.\n");
        return -1;
    }

    printf("Enter quantity: ");
    if (scanf("%d", &product.quantity) != 1) {
        printf("Invalid quantity.\n");
        return -1;
    }

    printf("Enter price: ");
    if (scanf("%lf", &product.price) != 1) {
        printf("Invalid price.\n");
        return -1;
    }

    /*
     * Move to the end of the file.
     */
    if (lseek(fd, 0, SEEK_END) == (off_t)-1)    {
        perror("lseek");
        return -1;
    }

    bytes_written = write(fd, &product, sizeof(Product));

    if (bytes_written != (ssize_t)sizeof(Product))  {
        perror("write");
        return -1;
    }

    printf("Product added successfully.\n");

    return 0;
}

/*
 * @brief Show a product by its index.
 *
 * @param fd File descriptor of the opened file.
 * @return 0 on success, -1 on error. 
 */
static int show_product(int fd) {
    Product product;
    int index;
    off_t offset;
    ssize_t bytes_read;

    printf("Enter product index: ");

    if (scanf("%d", &index) != 1) {
        printf("Invalid index.\n");
        return -1;
    }

    if (index < 0)  {
        printf("Index must be >= 0.\n");
        return -1;
    }

    /*
     * Calculate the byte offset of the requested record.
     *
     * Example:
     * index = 2
     *
     * offset = 2 * sizeof(Product)
     */
    offset = (off_t)index * sizeof(Product);

    if (lseek(fd, offset, SEEK_SET) == (off_t)-1)   {
        perror("lseek");
        return -1;
    }

    bytes_read = read(fd, &product, sizeof(Product));

    if (bytes_read == 0) {
        printf("Product index %d not found.\n", index);
        return 0;
    }

    if (bytes_read != (ssize_t)sizeof(Product)) {
        printf("Error: incomplete product record.\n");
        return -1;
    }

    printf("\n--- Product %d ---\n", index);
    printf("ID       : %d\n", product.id);
    printf("Name     : %s\n", product.name);
    printf("Quantity : %d\n", product.quantity);
    printf("Price    : %.2f\n", product.price);

    return 0;
}

/*
 * @brief Update the quantity of a product by its index.
 *
 * @param fd File descriptor of the opened file.
 * @return 0 on success, -1 on error.
 */
static int update_quantity(int fd)  {
    int index;
    int new_quantity;
    off_t offset;
    off_t field_offset;
    ssize_t bytes_written;

    printf("Enter product index: ");

    if (scanf("%d", &index) != 1)  {
        printf("Invalid index.\n");
        return -1;
    }

    if (index < 0)  {
        printf("Index must be >= 0.\n");
        return -1;
    }

    printf("Enter new quantity: ");

    if (scanf("%d", &new_quantity) != 1)  {
        printf("Invalid quantity.\n");
        return -1;
    }

    /*
     * Offset of the target product.
     */
    offset = (off_t)index * sizeof(Product);

    /*
     * Offset of quantity inside the target Product.
     */
    field_offset = offset + offsetof(Product, quantity);

    /*
     * Move directly to the quantity field.
     */
    if (lseek(fd, field_offset, SEEK_SET) == (off_t)-1) {
        perror("lseek");
        return -1;
    }

    /*
     * Write ONLY quantity.
     *
     * The rest of the Product remains unchanged.
     */
    bytes_written = write(fd, &new_quantity, sizeof(new_quantity));

    if (bytes_written != (ssize_t)sizeof(new_quantity)) {
        perror("write");
        return -1;
    }

    printf("Quantity updated successfully.\n");

    return 0;
}

/*
 * @brief List all products in the file.
 *
 * @param fd File descriptor of the opened file.
 * @return 0 on success, -1 on error.
 */
static int list_products(int fd) {
    Product product;
    ssize_t bytes_read;
    int index = 0;

    /*
     * Start from the beginning of the file.
     */
    if (lseek(fd, 0, SEEK_SET) == (off_t)-1) {
        perror("lseek");
        return -1;
    }

    while (1) {
        bytes_read = read(fd, &product, sizeof(Product));

        if (bytes_read == 0) {
            break;
        }

        if (bytes_read != (ssize_t)sizeof(Product)) {
            printf("Error: incomplete product record.\n");
            return -1;
        }

        printf("\n--- Product %d ---\n", index);
        printf("ID       : %d\n", product.id);
        printf("Name     : %s\n", product.name);
        printf("Quantity : %d\n", product.quantity);
        printf("Price    : %.2f\n", product.price);

        index++;
    }

    if (index == 0) {
        printf("No products found.\n");
    }

    return 0;
}


