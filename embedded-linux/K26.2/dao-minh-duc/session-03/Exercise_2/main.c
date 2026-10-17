#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stddef.h>

#define FILE_NAME "products.dat"

typedef struct {
    int    id;
    char   name[64];
    int    quantity;
    double price;
} Product;


/*
 * Write all requested bytes to the file descriptor.
 */
static int write_full(int fd, const void *buffer, size_t size)
{
    size_t total = 0;
    const char *buf = (const char *)buffer;

    while (total < size) {
        ssize_t n = write(fd, buf + total, size - total);

        if (n < 0) {
            if (errno == EINTR)
                continue;

            return -1;
        }

        total += (size_t)n;
    }

    return 0;
}


/*
 * Remove remaining characters from stdin.
 */
static void clear_input_buffer(void)
{
    int c;

    while ((c = getchar()) != '\n' && c != EOF)
        ;
}


/*
 * Print one product.
 */
static void print_product(const Product *product, int index)
{
    printf("Index    : %d\n", index);
    printf("ID       : %d\n", product->id);
    printf("Name     : %s\n", product->name);
    printf("Quantity : %d\n", product->quantity);
    printf("Price    : %.2f\n", product->price);
    printf("-------------------------\n");
}


/*
 * Add a new product at the end of the file.
 */
static void add_product(int fd)
{
    Product product;

    memset(&product, 0, sizeof(Product));

    printf("Enter ID: ");

    if (scanf("%d", &product.id) != 1) {
        printf("Invalid ID.\n");
        clear_input_buffer();
        return;
    }

    clear_input_buffer();

    printf("Enter name: ");

    if (fgets(product.name, sizeof(product.name), stdin) == NULL) {
        printf("Failed to read name.\n");
        return;
    }

    product.name[strcspn(product.name, "\n")] = '\0';

    printf("Enter quantity: ");

    if (scanf("%d", &product.quantity) != 1) {
        printf("Invalid quantity.\n");
        clear_input_buffer();
        return;
    }

    printf("Enter price: ");

    if (scanf("%lf", &product.price) != 1) {
        printf("Invalid price.\n");
        clear_input_buffer();
        return;
    }

    clear_input_buffer();

    /*
     * Move to the end of the file.
     */
    if (lseek(fd, 0, SEEK_END) == (off_t)-1) {
        perror("lseek");
        return;
    }

    /*
     * Append one complete Product record.
     */
    if (write_full(fd, &product, sizeof(Product)) < 0) {
        perror("write");
        return;
    }

    printf("Product added successfully.\n");
}


/*
 * Show one product by its index.
 *
 * The record offset is:
 *
 *     index * sizeof(Product)
 */
static void show_product(int fd)
{
    Product product;
    int index;
    off_t offset;
    ssize_t bytes_read;

    printf("Enter product index: ");

    if (scanf("%d", &index) != 1) {
        printf("Invalid index.\n");
        clear_input_buffer();
        return;
    }

    clear_input_buffer();

    if (index < 0) {
        printf("Invalid index.\n");
        return;
    }

    /*
     * Calculate the exact byte offset of the record.
     */
    offset = (off_t)index * sizeof(Product);

    /*
     * Jump directly to the requested record.
     */
    if (lseek(fd, offset, SEEK_SET) == (off_t)-1) {
        perror("lseek");
        return;
    }

    /*
     * Read exactly one Product.
     */
    bytes_read = read(fd, &product, sizeof(Product));

    if (bytes_read < 0) {
        if (errno == EINTR) {
            /*
             * For this simple operation, report an error
             * if the read is interrupted.
             */
            printf("Read interrupted. Please try again.\n");
            return;
        }

        perror("read");
        return;
    }

    if (bytes_read == 0) {
        printf("Product index %d not found.\n", index);
        return;
    }

    if ((size_t)bytes_read != sizeof(Product)) {
        const char msg[] = "Error: incomplete product record.\n";
        write(STDERR_FILENO, msg, sizeof(msg) - 1);
        return;
    }

    print_product(&product, index);
}


/*
 * Update only the quantity field of a product.
 *
 * Important:
 * We DO NOT read or rewrite the whole Product.
 *
 * Offset:
 *
 *     record_offset + offsetof(Product, quantity)
 */
static void update_quantity(int fd)
{
    int index;
    int new_quantity;
    off_t record_offset;
    off_t field_offset;

    printf("Enter product index: ");

    if (scanf("%d", &index) != 1) {
        printf("Invalid index.\n");
        clear_input_buffer();
        return;
    }

    if (index < 0) {
        printf("Invalid index.\n");
        clear_input_buffer();
        return;
    }

    printf("Enter new quantity: ");

    if (scanf("%d", &new_quantity) != 1) {
        printf("Invalid quantity.\n");
        clear_input_buffer();
        return;
    }

    clear_input_buffer();

    /*
     * Calculate the byte offset of the target record.
     */
    record_offset = (off_t)index * sizeof(Product);

    /*
     * Calculate the exact byte offset of the quantity field.
     */
    field_offset =
        record_offset + offsetof(Product, quantity);

    /*
     * Jump directly to Product.quantity.
     */
    if (lseek(fd, field_offset, SEEK_SET) == (off_t)-1) {
        perror("lseek");
        return;
    }

    /*
     * Write ONLY the quantity field.
     *
     * Do not write the entire Product.
     */
    if (write_full(fd, &new_quantity, sizeof(new_quantity)) < 0) {
        perror("write");
        return;
    }

    printf("Quantity updated successfully.\n");
}


/*
 * List all products sequentially.
 *
 * Unlike Show/Update, this operation is allowed to read
 * every record one by one.
 */
static void list_products(int fd)
{
    Product product;
    ssize_t bytes_read;
    int index = 0;

    /*
     * Start from the beginning.
     */
    if (lseek(fd, 0, SEEK_SET) == (off_t)-1) {
        perror("lseek");
        return;
    }

    while (1) {
        bytes_read = read(fd, &product, sizeof(Product));

        if (bytes_read < 0) {
            if (errno == EINTR)
                continue;

            perror("read");
            return;
        }

        /*
         * End of file.
         */
        if (bytes_read == 0)
            break;

        /*
         * A complete record must have exactly
         * sizeof(Product) bytes.
         */
        if ((size_t)bytes_read != sizeof(Product)) {
            const char msg[] = "Error: incomplete product record.\n";
            write(STDERR_FILENO, msg, sizeof(msg) - 1);
            return;
        }

        print_product(&product, index);

        index++;
    }

    if (index == 0) {
        printf("No products found.\n");
    } else {
        printf("Total products: %d\n", index);
    }
}


/*
 * Display the menu.
 */
static void print_menu(void)
{
    printf("\n");
    printf("===== Product Management =====\n");
    printf("1. Add product\n");
    printf("2. Show product by index\n");
    printf("3. Update quantity by index\n");
    printf("4. List all products\n");
    printf("5. Exit\n");
    printf("Choose an option: ");
}


int main(void)
{
    int fd;
    int choice;

    /*
     * Open products.dat for reading and writing.
     * Create the file if it does not exist.
     */
    fd = open(FILE_NAME, O_RDWR | O_CREAT, 0644);

    if (fd < 0) {
        perror("open");
        return EXIT_FAILURE;
    }

    while (1) {
        print_menu();

        if (scanf("%d", &choice) != 1) {
            printf("Invalid choice.\n");
            clear_input_buffer();
            continue;
        }

        clear_input_buffer();

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
            if (close(fd) < 0) {
                perror("close");
                return EXIT_FAILURE;
            }

            printf("Goodbye!\n");
            return EXIT_SUCCESS;

        default:
            printf("Invalid choice. Please select 1-5.\n");
            break;
        }
    }

    return EXIT_SUCCESS;
}
