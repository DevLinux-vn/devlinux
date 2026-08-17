#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#define FILE_NAME "products.dat"
#define BUFFER_SIZE 256

typedef struct {
    int    id;
    char   name[64];
    int    quantity;
    double price;
} Product;


/*
 * Read one line from stdin using read().
 */
static void read_line(char *buffer, size_t size)
{
    size_t i = 0;
    char c;

    while (i < size - 1) {
        ssize_t n = read(STDIN_FILENO, &c, 1);

        if (n <= 0) {
            break;
        }

        if (c == '\n') {
            break;
        }

        buffer[i++] = c;
    }

    buffer[i] = '\0';
}


/*
 * Read an integer from stdin.
 */
static int read_int(void)
{
    char buffer[BUFFER_SIZE];

    read_line(buffer, sizeof(buffer));

    return atoi(buffer);
}


/*
 * Read a double from stdin.
 */
static double read_double(void)
{
    char buffer[BUFFER_SIZE];

    read_line(buffer, sizeof(buffer));

    return strtod(buffer, NULL);
}


/*
 * Print a string using write().
 */
static void print_string(const char *str)
{
    write(STDOUT_FILENO, str, strlen(str));
}


/*
 * Print one product.
 */
static void print_product(const Product *product)
{
    char buffer[BUFFER_SIZE];

    int len = snprintf(
        buffer,
        sizeof(buffer),
        "ID: %d\n"
        "Name: %s\n"
        "Quantity: %d\n"
        "Price: %.2f\n"
        "--------------------\n",
        product->id,
        product->name,
        product->quantity,
        product->price
    );

    write(STDOUT_FILENO, buffer, len);
}


/*
 * Option 1:
 * Add a product at the end of the file.
 */
static void add_product(int fd)
{
    Product product;

    memset(&product, 0, sizeof(Product));

    print_string("Enter product ID: ");
    product.id = read_int();

    print_string("Enter product name: ");
    read_line(product.name, sizeof(product.name));

    print_string("Enter quantity: ");
    product.quantity = read_int();

    print_string("Enter price: ");
    product.price = read_double();

    /*
     * Move to the end of the file.
     */
    if (lseek(fd, 0, SEEK_END) == (off_t)-1) {
        print_string("Error: lseek failed.\n");
        return;
    }

    ssize_t written = write(fd, &product, sizeof(Product));

    if (written != sizeof(Product)) {
        print_string("Error: failed to write product.\n");
        return;
    }

    print_string("Product added successfully.\n");
}


/*
 * Option 2:
 * Show one product by its index.
 */
static void show_product(int fd)
{
    Product product;
    int index;

    print_string("Enter product index: ");
    index = read_int();

    if (index < 0) {
        print_string("Error: index must be non-negative.\n");
        return;
    }

    /*
     * Calculate the byte offset of the requested record.
     */
    off_t offset = (off_t)index * sizeof(Product);

    /*
     * Jump directly to that record.
     */
    if (lseek(fd, offset, SEEK_SET) == (off_t)-1) {
        print_string("Error: lseek failed.\n");
        return;
    }

    /*
     * Read exactly one Product.
     */
    ssize_t bytes_read = read(fd, &product, sizeof(Product));

    if (bytes_read == 0) {
        print_string("Product not found.\n");
        return;
    }

    if (bytes_read != sizeof(Product)) {
        print_string("Error: incomplete product record.\n");
        return;
    }

    print_product(&product);
}


/*
 * Option 3:
 * Update only the quantity field of a product.
 */
static void update_quantity(int fd)
{
    int index;
    int new_quantity;

    print_string("Enter product index: ");
    index = read_int();

    if (index < 0) {
        print_string("Error: index must be non-negative.\n");
        return;
    }

    print_string("Enter new quantity: ");
    new_quantity = read_int();

    /*
     * Calculate the byte offset of the target record.
     */
    off_t offset = (off_t)index * sizeof(Product);

    /*
     * Calculate the exact byte offset of the quantity field.
     */
    off_t field_offset =
        offset + offsetof(Product, quantity);

    /*
     * Jump directly to the quantity field.
     */
    if (lseek(fd, field_offset, SEEK_SET) == (off_t)-1) {
        print_string("Error: lseek failed.\n");
        return;
    }

    /*
     * Write ONLY the quantity field.
     */
    ssize_t written = write(
        fd,
        &new_quantity,
        sizeof(new_quantity)
    );

    if (written != sizeof(new_quantity)) {
        print_string("Error: failed to update quantity.\n");
        return;
    }

    print_string("Quantity updated successfully.\n");
}


/*
 * Option 4:
 * List all products sequentially.
 */
static void list_products(int fd)
{
    Product product;
    ssize_t bytes_read;
    int index = 0;

    /*
     * Start from the beginning of the file.
     */
    if (lseek(fd, 0, SEEK_SET) == (off_t)-1) {
        print_string("Error: lseek failed.\n");
        return;
    }

    while (1) {

        bytes_read = read(fd, &product, sizeof(Product));

        if (bytes_read == 0) {
            break;
        }

        if (bytes_read < 0) {
            print_string("Error: read failed.\n");
            return;
        }

        if (bytes_read != sizeof(Product)) {
            print_string("Error: incomplete product record.\n");
            return;
        }

        char buffer[64];

        int len = snprintf(
            buffer,
            sizeof(buffer),
            "Product index: %d\n",
            index
        );

        write(STDOUT_FILENO, buffer, len);

        print_product(&product);

        index++;
    }

    if (index == 0) {
        print_string("No products found.\n");
    }
}


int main(void)
{
    int fd;
    int choice;

    /*
     * Open existing products.dat or create it.
     */
    fd = open(FILE_NAME, O_RDWR | O_CREAT, 0644);

    if (fd == -1) {
        print_string("Error: cannot open products.dat\n");
        return 1;
    }

    while (1) {

        print_string(
            "\n"
            "===== Product Management =====\n"
            "1. Add product\n"
            "2. Show product by index\n"
            "3. Update quantity by index\n"
            "4. List all products\n"
            "5. Exit\n"
            "Choose an option: "
        );

        choice = read_int();

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
                print_string("Goodbye!\n");
                return 0;

            default:
                print_string(
                    "Invalid option. Please try again.\n"
                );
                break;
        }
    }
}