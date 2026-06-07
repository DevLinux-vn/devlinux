#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stddef.h>
#include <sys/stat.h>

#define FILE_NAME "products.dat"
#define MENU_EXIT_CHOICE 5

typedef struct {
    int    id;
    char   name[64];
    int    quantity;
    double price;
} Product;

static void flush_stdin(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

static void add_product(int fd) {
    Product p;
    memset(&p, 0, sizeof(Product));

    printf("Enter Product ID: ");
    if (scanf("%d", &p.id) != 1) { flush_stdin(); return; }
    flush_stdin();

    printf("Enter Product Name: ");
    if (fgets(p.name, sizeof(p.name), stdin)) {
        p.name[strcspn(p.name, "\n")] = 0;
    }

    printf("Enter Quantity: ");
    if (scanf("%d", &p.quantity) != 1) { flush_stdin(); return; }

    printf("Enter Price: ");
    if (scanf("%lf", &p.price) != 1) { flush_stdin(); return; }
    flush_stdin();

    if (lseek(fd, 0, SEEK_END) == (off_t)-1) {
        perror("lseek to end failed");
        return;
    }

    if (write(fd, &p, sizeof(Product)) != (ssize_t)sizeof(Product)) {
        perror("Error saving product");
    } else {
        printf("Product appended successfully.\n");
    }
}

static void show_product_by_index(int fd) {
    int index;
    Product p;

    printf("Enter product index (0-based): ");
    if (scanf("%d", &index) != 1) { flush_stdin(); return; }
    flush_stdin();

    const off_t offset = (off_t)index * sizeof(Product);
    
    const off_t file_size = lseek(fd, 0, SEEK_END);
    if (file_size == (off_t)-1) {
        perror("lseek file size check failed");
        return;
    }
    
    if (offset >= file_size || offset < 0) {
        printf("Error: Index out of bounds!\n");
        return;
    }

    if (lseek(fd, offset, SEEK_SET) == (off_t)-1) {
        perror("lseek to target offset failed");
        return;
    }

    if (read(fd, &p, sizeof(Product)) == (ssize_t)sizeof(Product)) {
        printf("\nProduct at index %d:\n", index);
        printf("ID: %d\nName: %s\nQuantity: %d\nPrice: %.2f\n", p.id, p.name, p.quantity, p.price);
    } else {
        perror("Error reading product record");
    }
}

static void update_quantity_by_index(int fd) {
    int index, new_qty;

    printf("Enter product index to update (0-based): ");
    if (scanf("%d", &index) != 1) { flush_stdin(); return; }
    
    printf("Enter new quantity: ");
    if (scanf("%d", &new_qty) != 1) { flush_stdin(); return; }
    flush_stdin();

    const off_t offset = (off_t)index * sizeof(Product);
    
    const off_t file_size = lseek(fd, 0, SEEK_END);
    if (file_size == (off_t)-1) {
        perror("lseek file size check failed");
        return;
    }

    if (offset >= file_size || offset < 0) {
        printf("Error: Index out of bounds!\n");
        return;
    }

    const off_t field_offset = offset + offsetof(Product, quantity);
    
    if (lseek(fd, field_offset, SEEK_SET) == (off_t)-1) {
        perror("lseek to field offset failed");
        return;
    }

    if (write(fd, &new_qty, sizeof(int)) == (ssize_t)sizeof(int)) {
        printf("Quantity updated successfully via lseek.\n");
    } else {
        perror("Failed to update quantity");
    }
}

static void list_all_products(int fd) {
    Product p;
    if (lseek(fd, 0, SEEK_SET) == (off_t)-1) {
        perror("lseek to start failed");
        return;
    }
    int idx = 0;

    printf("\n--- Product List ---\n");
    printf("%-5s %-5s %-20s %-10s %-10s\n", "Idx", "ID", "Name", "Qty", "Price");
    while (1) {
        const ssize_t bytes_read = read(fd, &p, sizeof(Product));
        if (bytes_read < 0) {
            perror("Error reading products");
            break;
        }
        if (bytes_read == 0) {
            break; /* EOF */
        }
        if (bytes_read == (ssize_t)sizeof(Product)) {
            printf("%-5d %-5d %-20s %-10d %-10.2f\n", idx++, p.id, p.name, p.quantity, p.price);
        }
    }
}

int main(void) {
    int fd = open(FILE_NAME, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd < 0) {
        perror("Failed to open file");
        return EXIT_FAILURE;
    }

    int choice;
    do {
        printf("\n=== Product Management Menu ===\n");
        printf("1. Add product\n");
        printf("2. Show product by index\n");
        printf("3. Update quantity by index\n");
        printf("4. List all products\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input!\n");
            flush_stdin();
            continue;
        }
        flush_stdin();

        switch (choice) {
            case 1: add_product(fd); break;
            case 2: show_product_by_index(fd); break;
            case 3: update_quantity_by_index(fd); break;
            case 4: list_all_products(fd); break;
            case 5: printf("Exiting...\n"); break;
            default: printf("Invalid choice!\n");
        }
    } while (choice != MENU_EXIT_CHOICE);

    if (close(fd) < 0) {
        perror("Error closing file descriptor");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}