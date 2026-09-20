#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>

#define FILE_NAME "products.dat"

typedef struct {
    int    id;
    char   name[64];
    int    quantity;
    double price;
} Product;

static void add_product(int fd) {
    Product p;
    memset(&p, 0, sizeof(Product));

    printf("Enter Product ID: ");
    if (scanf("%d", &p.id) != 1) {
        printf("Invalid ID\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    printf("Enter Product Name: ");
    if (fgets(p.name, sizeof(p.name), stdin) != NULL) {
        p.name[strcspn(p.name, "\r\n")] = '\0';
    }

    printf("Enter Quantity: ");
    if (scanf("%d", &p.quantity) != 1) {
        printf("Invalid Quantity\n");
        while (getchar() != '\n');
        return;
    }

    printf("Enter Price: ");
    if (scanf("%lf", &p.price) != 1) {
        printf("Invalid Price\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    if (lseek(fd, 0, SEEK_END) == (off_t)-1) {
        perror("lseek to end failed");
        return;
    }

    ssize_t bytes_written = write(fd, &p, sizeof(Product));
    if (bytes_written != (ssize_t)sizeof(Product)) {
        perror("write failed");
    } else {
        printf("Product added successfully!\n");
    }
}

static void show_product_by_index(int fd) {
    int index;
    printf("Enter record index (0-based): ");
    if (scanf("%d", &index) != 1 || index < 0) {
        printf("Invalid index\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    off_t offset = (off_t)index * sizeof(Product);
    if (lseek(fd, offset, SEEK_SET) == (off_t)-1) {
        perror("lseek failed");
        return;
    }

    Product p;
    ssize_t bytes_read = read(fd, &p, sizeof(Product));
    if (bytes_read == (ssize_t)sizeof(Product)) {
        printf("\n--- Product at Index %d ---\n", index);
        printf("ID:       %d\n", p.id);
        printf("Name:     %s\n", p.name);
        printf("Quantity: %d\n", p.quantity);
        printf("Price:    %.2f\n", p.price);
        printf("---------------------------\n");
    } else if (bytes_read == 0) {
        printf("Record index %d does not exist (out of range).\n", index);
    } else {
        perror("read failed");
    }
}

static void update_quantity_by_index(int fd) {
    int index;
    printf("Enter record index to update (0-based): ");
    if (scanf("%d", &index) != 1 || index < 0) {
        printf("Invalid index\n");
        while (getchar() != '\n');
        return;
    }

    off_t record_offset = (off_t)index * sizeof(Product);
    off_t file_size = lseek(fd, 0, SEEK_END);
    if (record_offset + (off_t)sizeof(Product) > file_size) {
        printf("Record index %d is out of range.\n", index);
        while (getchar() != '\n');
        return;
    }

    int new_quantity;
    printf("Enter new quantity: ");
    if (scanf("%d", &new_quantity) != 1) {
        printf("Invalid quantity\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    off_t field_offset = record_offset + offsetof(Product, quantity);
    if (lseek(fd, field_offset, SEEK_SET) == (off_t)-1) {
        perror("lseek to field failed");
        return;
    }

    ssize_t bytes_written = write(fd, &new_quantity, sizeof(int));
    if (bytes_written != (ssize_t)sizeof(int)) {
        perror("update quantity failed");
    } else {
        printf("Quantity for product at index %d updated successfully to %d!\n", index, new_quantity);
    }
}

static void list_all_products(int fd) {
    if (lseek(fd, 0, SEEK_SET) == (off_t)-1) {
        perror("lseek to start failed");
        return;
    }

    Product p;
    ssize_t bytes_read;
    int index = 0;

    printf("\n%-6s | %-5s | %-20s | %-8s | %-10s\n", "Index", "ID", "Name", "Quantity", "Price");
    printf("----------------------------------------------------------\n");

    while ((bytes_read = read(fd, &p, sizeof(Product))) == (ssize_t)sizeof(Product)) {
        printf("%-6d | %-5d | %-20s | %-8d | %-10.2f\n",
               index, p.id, p.name, p.quantity, p.price);
        index++;
    }

    if (bytes_read == -1) {
        perror("read failed");
    } else if (index == 0) {
        printf("No products available.\n");
    }
    printf("----------------------------------------------------------\n");
}

int main(void) {
    int fd = open(FILE_NAME, O_RDWR | O_CREAT, 0644);
    if (fd < 0) {
        perror("open failed");
        return 1;
    }

    int choice = 0;
    while (1) {
        printf("\n=== Product Inventory Management ===\n");
        printf("1. Add product\n");
        printf("2. Show product by index\n");
        printf("3. Update quantity by index\n");
        printf("4. List all products\n");
        printf("5. Exit\n");
        printf("Select an option (1-5): ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid choice. Try again.\n");
            while (getchar() != '\n');
            continue;
        }

        switch (choice) {
            case 1:
                add_product(fd);
                break;
            case 2:
                show_product_by_index(fd);
                break;
            case 3:
                update_quantity_by_index(fd);
                break;
            case 4:
                list_all_products(fd);
                break;
            case 5:
                close(fd);
                printf("Exiting...\n");
                return 0;
            default:
                printf("Invalid choice! Choose between 1 and 5.\n");
                break;
        }
    }

    close(fd);
    return 0;
}
