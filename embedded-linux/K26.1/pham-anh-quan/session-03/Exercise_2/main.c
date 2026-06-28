#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stddef.h>
#include <sys/types.h>

typedef struct {
    int id;
    char name[64];
    int quantity;
    double price;
} Product;

#define FILE_NAME "products.dat"

void add_product() {
    int fd = open(FILE_NAME, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd < 0) {
        perror("Error opening file");
        return;
    }

    Product p;
    printf("Enter Product ID: ");
    if (scanf("%d", &p.id) != 1) return;

    getchar(); // Xóa ký tự '\n' thừa
    printf("Enter Product Name: ");
    if (fgets(p.name, sizeof(p.name), stdin) != NULL) {
        p.name[strcspn(p.name, "\n")] = '\0';
    }

    printf("Enter Quantity: ");
    if (scanf("%d", &p.quantity) != 1) return;

    printf("Enter Price: ");
    if (scanf("%lf", &p.price) != 1) return;

    if (write(fd, &p, sizeof(Product)) < 0) {
        perror("Error writing product");
    } else {
        printf("Product added successfully.\n");
    }
    close(fd);
}

void show_product_by_index() {
    int fd = open(FILE_NAME, O_RDONLY);
    if (fd < 0) {
        printf("No product records found.\n");
        return;
    }

    int index;
    printf("Enter record index (0, 1, 2...): ");
    if (scanf("%d", &index) != 1) {
        close(fd);
        return;
    }

    // Dùng lseek nhảy thẳng đến vị trí record theo index: index * sizeof(Product)
    off_t offset = (off_t)index * sizeof(Product);
    if (lseek(fd, offset, SEEK_SET) < 0) {
        perror("Error seeking to index");
        close(fd);
        return;
    }

    Product p;
    // Đọc duy nhất 1 bản ghi tại vị trí đó
    if (read(fd, &p, sizeof(Product)) > 0) {
        printf("\n--- Product at Index %d ---\n", index);
        printf("ID: %d | Name: %s | Quantity: %d | Price: %.2f\n", p.id, p.name, p.quantity, p.price);
    } else {
        printf("Record at index %d does not exist.\n", index);
    }
    close(fd);
}

void update_quantity_by_index() {
    // Mở file bằng chế độ đọc/ghi (O_RDWR)
    int fd = open(FILE_NAME, O_RDWR);
    if (fd < 0) {
        printf("No product records found.\n");
        return;
    }

    int index, new_quantity;
    printf("Enter record index to update (0, 1, 2...): ");
    if (scanf("%d", &index) != 1) {
        close(fd);
        return;
    }

    printf("Enter new quantity: ");
    if (scanf("%d", &new_quantity) != 1) {
        close(fd);
        return;
    }

    // Tính toán chính xác offset của field quantity trong file
    off_t offset = (off_t)index * sizeof(Product);
    off_t field_offset = offset + offsetof(Product, quantity);

    // Nhảy trực tiếp đến field quantity
    if (lseek(fd, field_offset, SEEK_SET) < 0) {
        perror("Error seeking to field");
        close(fd);
        return;
    }

    // Chỉ ghi đè đúng giá trị mới của field quantity (4 bytes)
    if (write(fd, &new_quantity, sizeof(int)) < 0) {
        perror("Error updating quantity");
    } else {
        printf("Quantity updated successfully at index %d.\n", index);
    }
    close(fd);
}

void list_all_products() {
    int fd = open(FILE_NAME, O_RDONLY);
    if (fd < 0) {
        printf("No product records found.\n");
        return;
    }

    Product p;
    int index = 0;
    printf("\n--- Full Product List ---\n");
    while (read(fd, &p, sizeof(Product)) > 0) {
        printf("Index: %d | ID: %d | Name: %s | Quantity: %d | Price: %.2f\n", index, p.id, p.name, p.quantity, p.price);
        index++;
    }

    if (index == 0) {
        printf("No product records found.\n");
    }
    close(fd);
}

int main() {
    int choice;
    while (1) {
        printf("\n=== Product Management (lseek) ===\n");
        printf("1. Add product\n");
        printf("2. Show product by index\n");
        printf("3. Update quantity by index\n");
        printf("4. List all products\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            getchar();
            continue;
        }

        switch (choice) {
            case 1: add_product(); break;
            case 2: show_product_by_index(); break;
            case 3: update_quantity_by_index(); break;
            case 4: list_all_products(); break;
            case 5: printf("Exiting.\n"); return 0;
            default: printf("Invalid choice!\n");
        }
    }
    return 0;
}