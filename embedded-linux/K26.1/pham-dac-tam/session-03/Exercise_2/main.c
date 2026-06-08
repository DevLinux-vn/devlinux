#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stddef.h>

typedef struct {
    int    id;
    char   name[64];
    int    quantity;
    double price;
} Product;

#define DATA_FILE "products.dat"

// Mở file dữ liệu
int open_data_file() {
    int fd = open(DATA_FILE, O_RDWR | O_CREAT, 0644);
    if (fd == -1) {
        perror("open");
        exit(1);
    }
    return fd;
}

// Tính số lượng record hiện tại trong file
int count_products(int fd) {
    off_t file_size = lseek(fd, 0, SEEK_END);
    if (file_size == -1) {
        perror("lseek");
        return -1;
    }
    return file_size / sizeof(Product);
}

// Thêm product mới
void add_product(int fd) {
    Product product;
    
    printf("\n=== Add Product ===\n");
    printf("Enter ID: ");
    scanf("%d", &product.id);
    printf("Enter Name: ");
    scanf("%s", product.name);
    printf("Enter Quantity: ");
    scanf("%d", &product.quantity);
    printf("Enter Price: ");
    scanf("%lf", &product.price);
    
    // Di chuyển đến cuối file
    if (lseek(fd, 0, SEEK_END) == -1) {
        perror("lseek");
        return;
    }
    
    // Ghi product mới
    if (write(fd, &product, sizeof(Product)) == -1) {
        perror("write");
        return;
    }
    
    printf("✓ Product added successfully!\n");
}

// Hiển thị product theo index
void show_product_by_index(int fd) {
    int index;
    int total_products = count_products(fd);
    Product product;
    
    printf("\n=== Show Product by Index ===\n");
    printf("Total products: %d\n", total_products);
    
    if (total_products == 0) {
        printf("No products in file.\n");
        return;
    }
    
    printf("Enter index (0-%d): ", total_products - 1);
    scanf("%d", &index);
    
    if (index < 0 || index >= total_products) {
        printf("✗ Invalid index!\n");
        return;
    }
    
    // Công thức: byte offset = index × sizeof(Product)
    off_t offset = (off_t)index * sizeof(Product);
    
    if (lseek(fd, offset, SEEK_SET) == -1) {
        perror("lseek");
        return;
    }
    
    if (read(fd, &product, sizeof(Product)) != sizeof(Product)) {
        perror("read");
        return;
    }
    
    printf("\nProduct at index %d:\n", index);
    printf("  ID:       %d\n", product.id);
    printf("  Name:     %s\n", product.name);
    printf("  Quantity: %d\n", product.quantity);
    printf("  Price:    $%.2f\n", product.price);
}

// Cập nhật số lượng của product theo index
void update_quantity_by_index(int fd) {
    int index, new_quantity;
    int total_products = count_products(fd);
    Product product;
    
    printf("\n=== Update Quantity by Index ===\n");
    printf("Total products: %d\n", total_products);
    
    if (total_products == 0) {
        printf("No products in file.\n");
        return;
    }
    
    printf("Enter index (0-%d): ", total_products - 1);
    scanf("%d", &index);
    
    if (index < 0 || index >= total_products) {
        printf("✗ Invalid index!\n");
        return;
    }
    
    printf("Enter new quantity: ");
    scanf("%d", &new_quantity);
    
    // Byte offset của record
    off_t record_offset = (off_t)index * sizeof(Product);
    
    // Byte offset của field 'quantity' trong struct
    // offsetof() tính offset của field từ đầu struct
    off_t quantity_offset = record_offset + offsetof(Product, quantity);
    
    // Seek đến vị trí của field quantity
    if (lseek(fd, quantity_offset, SEEK_SET) == -1) {
        perror("lseek");
        return;
    }
    
    // Ghi chỉ field quantity (không viết lại toàn bộ record)
    if (write(fd, &new_quantity, sizeof(int)) == -1) {
        perror("write");
        return;
    }
    
    printf("✓ Quantity updated successfully!\n");
}

// Hiển thị tất cả products
void list_all_products(int fd) {
    Product product;
    int count = 0;
    int total_products = count_products(fd);
    
    printf("\n=== All Products ===\n");
    
    if (total_products == 0) {
        printf("No products found.\n");
        return;
    }
    
    // Quay về đầu file
    if (lseek(fd, 0, SEEK_SET) == -1) {
        perror("lseek");
        return;
    }
    
    // Đọc từng product
    while (read(fd, &product, sizeof(Product)) == sizeof(Product)) {
        printf("\n[%d] ID: %d\n", count, product.id);
        printf("    Name:     %s\n", product.name);
        printf("    Quantity: %d\n", product.quantity);
        printf("    Price:    $%.2f\n", product.price);
        count++;
    }
}

void show_menu() {
    printf("\n====== PRODUCT MANAGER ======\n");
    printf("1. Add product\n");
    printf("2. Show product by index\n");
    printf("3. Update quantity by index\n");
    printf("4. List all products\n");
    printf("5. Exit\n");
    printf("=============================\n");
    printf("Enter choice: ");
}

int main() {
    int fd = open_data_file();
    int choice;
    
    printf("sizeof(Product) = %zu bytes\n", sizeof(Product));
    
    while (1) {
        show_menu();
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
                printf("\n✓ Goodbye!\n");
                return 0;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
    
    return 0;
}
