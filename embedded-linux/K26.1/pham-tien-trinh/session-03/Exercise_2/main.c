#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stddef.h>

#define DATA_FILE "products.dat"

typedef struct {
    int    id;
    char   name[64];
    int    quantity;
    double price;
} Product;

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void print_product(Product p, int index) {
    printf("\nProduct index: %d\n", index);
    printf("ID       : %d\n", p.id);
    printf("Name     : %s\n", p.name);
    printf("Quantity : %d\n", p.quantity);
    printf("Price    : %.2lf\n", p.price);
}

void add_product(int fd) {
    Product p;

    printf("Enter product ID: ");
    scanf("%d", &p.id);
    clear_input_buffer();

    printf("Enter product name: ");
    fgets(p.name, sizeof(p.name), stdin);
    p.name[strcspn(p.name, "\n")] = '\0';

    printf("Enter quantity: ");
    scanf("%d", &p.quantity);

    printf("Enter price: ");
    scanf("%lf", &p.price);

    lseek(fd, 0, SEEK_END);

    ssize_t bytes_written = write(fd, &p, sizeof(Product));

    if (bytes_written != sizeof(Product)) {
        perror("write");
        return;
    }

    printf("Product added successfully.\n");
}

void show_product_by_index(int fd) {
    int index;
    Product p;

    printf("Enter product index: ");
    scanf("%d", &index);

    if (index < 0) {
        printf("Invalid index.\n");
        return;
    }

    off_t offset = (off_t)index * sizeof(Product);

    if (lseek(fd, offset, SEEK_SET) == -1) {
        perror("lseek");
        return;
    }

    ssize_t bytes_read = read(fd, &p, sizeof(Product));

    if (bytes_read == sizeof(Product)) {
        print_product(p, index);
    } else {
        printf("Product at index %d not found.\n", index);
    }
}

void update_quantity_by_index(int fd) {
    int index;
    int new_quantity;

    printf("Enter product index to update: ");
    scanf("%d", &index);

    if (index < 0) {
        printf("Invalid index.\n");
        return;
    }

    printf("Enter new quantity: ");
    scanf("%d", &new_quantity);

    off_t record_offset = (off_t)index * sizeof(Product);
    off_t quantity_offset = record_offset + offsetof(Product, quantity);

    if (lseek(fd, quantity_offset, SEEK_SET) == -1) {
        perror("lseek");
        return;
    }

    ssize_t bytes_written = write(fd, &new_quantity, sizeof(int));

    if (bytes_written != sizeof(int)) {
        printf("Failed to update quantity. Product may not exist.\n");
        return;
    }

    printf("Quantity updated successfully.\n");
}

void list_all_products(int fd) {
    Product p;
    int index = 0;

    lseek(fd, 0, SEEK_SET);

    printf("\n===== Product List =====\n");

    while (read(fd, &p, sizeof(Product)) == sizeof(Product)) {
        print_product(p, index);
        printf("------------------------\n");
        index++;
    }

    if (index == 0) {
        printf("No products found.\n");
    }
}

int main() {
    int fd;
    int choice;

    fd = open(DATA_FILE, O_RDWR | O_CREAT, 0644);

    if (fd == -1) {
        perror("open");
        return 1;
    }

    while (1) {
        printf("\n===== Product Management =====\n");
        printf("1. Add product\n");
        printf("2. Show product by index\n");
        printf("3. Update quantity by index\n");
        printf("4. List all products\n");
        printf("5. Exit\n");
        printf("Choose an option: ");

        scanf("%d", &choice);

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
                printf("Program exited.\n");
                return 0;

            default:
                printf("Invalid choice. Please try again.\n");
                break;
        }
    }
}