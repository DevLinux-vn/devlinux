#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define DATA_FILE "products.dat"

typedef struct {
    int id;
    char name[64];
    int quantity;
    double price;
} Product;

static void strip_newline(char *s)
{
    size_t len = strlen(s);

    if (len > 0 && s[len - 1] == '\n') {
        s[len - 1] = '\0';
    }
}

static int read_line(const char *prompt, char *buf, size_t size)
{
    printf("%s", prompt);
    fflush(stdout);

    if (fgets(buf, size, stdin) == NULL) {
        return 0;
    }

    strip_newline(buf);
    return 1;
}

static int read_int(const char *prompt, int *value)
{
    char buf[128];
    char extra;

    while (read_line(prompt, buf, sizeof(buf))) {
        if (sscanf(buf, "%d %c", value, &extra) == 1) {
            return 1;
        }
        printf("Invalid number. Please try again.\n");
    }

    return 0;
}

static int read_double(const char *prompt, double *value)
{
    char buf[128];
    char extra;

    while (read_line(prompt, buf, sizeof(buf))) {
        if (sscanf(buf, "%lf %c", value, &extra) == 1) {
            return 1;
        }
        printf("Invalid number. Please try again.\n");
    }

    return 0;
}

static ssize_t write_full(int fd, const void *buf, size_t count)
{
    const char *p = buf;
    size_t written = 0;

    while (written < count) {
        ssize_t n = write(fd, p + written, count - written);
        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }
        written += (size_t)n;
    }

    return (ssize_t)written;
}

static int product_count(int fd)
{
    struct stat st;

    if (fstat(fd, &st) < 0) {
        perror("fstat");
        return -1;
    }

    return (int)(st.st_size / (off_t)sizeof(Product));
}

static int read_product_at(int fd, int index, Product *product)
{
    off_t offset = (off_t)index * (off_t)sizeof(Product);
    ssize_t n;

    if (lseek(fd, offset, SEEK_SET) == (off_t)-1) {
        perror("lseek");
        return 0;
    }

    n = read(fd, product, sizeof(*product));
    if (n < 0) {
        perror("read");
        return 0;
    }

    return n == (ssize_t)sizeof(*product);
}

static void print_product(const Product *product)
{
    printf("ID: %d | Name: %s | Quantity: %d | Price: %.2f\n",
           product->id,
           product->name,
           product->quantity,
           product->price);
}

static void add_product(int fd)
{
    Product product;

    memset(&product, 0, sizeof(product));

    if (!read_int("ID: ", &product.id) ||
        !read_line("Name: ", product.name, sizeof(product.name)) ||
        !read_int("Quantity: ", &product.quantity) ||
        !read_double("Price: ", &product.price)) {
        printf("Input ended. Product was not added.\n");
        return;
    }

    if (lseek(fd, 0, SEEK_END) == (off_t)-1) {
        perror("lseek");
        return;
    }

    if (write_full(fd, &product, sizeof(product)) != (ssize_t)sizeof(product)) {
        perror("write");
        return;
    }

    printf("Product added successfully.\n");
}

static void show_product(int fd)
{
    Product product;
    int index;
    int count = product_count(fd);

    if (count < 0) {
        return;
    }

    if (!read_int("Index (0-based): ", &index)) {
        printf("Input ended.\n");
        return;
    }

    if (index < 0 || index >= count) {
        printf("Invalid index. Current record count: %d\n", count);
        return;
    }

    if (read_product_at(fd, index, &product)) {
        printf("[%d] ", index);
        print_product(&product);
    }
}

static void update_quantity(int fd)
{
    int index;
    int quantity;
    int count = product_count(fd);
    off_t offset;

    if (count < 0) {
        return;
    }

    if (!read_int("Index (0-based): ", &index) ||
        !read_int("New quantity: ", &quantity)) {
        printf("Input ended.\n");
        return;
    }

    if (index < 0 || index >= count) {
        printf("Invalid index. Current record count: %d\n", count);
        return;
    }

    offset = (off_t)index * (off_t)sizeof(Product) +
             (off_t)offsetof(Product, quantity);

    if (lseek(fd, offset, SEEK_SET) == (off_t)-1) {
        perror("lseek");
        return;
    }

    if (write_full(fd, &quantity, sizeof(quantity)) != (ssize_t)sizeof(quantity)) {
        perror("write");
        return;
    }

    printf("Quantity updated successfully.\n");
}

static void list_products(int fd)
{
    Product product;
    int index = 0;

    if (lseek(fd, 0, SEEK_SET) == (off_t)-1) {
        perror("lseek");
        return;
    }

    printf("\n========== PRODUCT LIST ==========\n");
    for (;;) {
        ssize_t n = read(fd, &product, sizeof(product));
        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }
            perror("read");
            break;
        }
        if (n == 0) {
            break;
        }
        if (n != (ssize_t)sizeof(product)) {
            printf("Warning: ignored a partial/corrupted record.\n");
            break;
        }

        printf("[%d] ", index);
        print_product(&product);
        index++;
    }

    if (index == 0) {
        printf("No products found.\n");
    }
    printf("==================================\n\n");
}

static void print_menu(void)
{
    printf("1. Add product\n");
    printf("2. Show product by index\n");
    printf("3. Update quantity by index\n");
    printf("4. List all products\n");
    printf("5. Exit\n");
    printf("Choose: ");
    fflush(stdout);
}

int main(void)
{
    int fd = open(DATA_FILE, O_RDWR | O_CREAT, 0644);
    char choice[32];

    if (fd < 0) {
        perror("open");
        return 1;
    }

    printf("Product manager using lseek (%s)\n\n", DATA_FILE);

    for (;;) {
        print_menu();

        if (fgets(choice, sizeof(choice), stdin) == NULL) {
            break;
        }

        switch (atoi(choice)) {
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
            printf("Goodbye!\n");
            return 0;
        default:
            printf("Invalid choice.\n");
            break;
        }
    }

    close(fd);
    return 0;
}
