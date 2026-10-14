#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h> 
#include <unistd.h>
#include <stddef.h>

// Định nghĩa cấu trúc Sản phẩm có kích thước cố định
typedef struct {
    int    id;
    char   name[64];
    int    quantity;
    double price;
} Product;

// Chức năng 1: Thêm sản phẩm mới (Ghi nối đuôi vào cuối file)
void add_product() {
    Product p;
    printf("\n THÊM SẢN PHẨM MỚI \n");
    printf("Nhập ID sản phẩm: ");
    scanf("%d", &p.id);
    getchar();
    
    printf("Nhập tên sản phẩm: ");
    fgets(p.name, sizeof(p.name), stdin);
    p.name[strcspn(p.name, "\n")] = 0;
    
    printf("Nhập số lượng: ");
    scanf("%d", &p.quantity);
    
    printf("Nhập đơn giá: ");
    scanf("%lf", &p.price);

    int fd = open("products.dat", O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd < 0) {
        perror("Lỗi mở file");
        return;
    }

    if (write(fd, &p, sizeof(Product)) != sizeof(Product)) {
        perror("Lỗi ghi dữ liệu");
    } else {
        printf(" Đã thêm sản phẩm thành công vào cuối file!\n");
    }
    close(fd);
}

// Chức năng 2: Hiển thị sản phẩm theo chỉ mục (Index) dùng lseek
void show_product_by_index() {
    int index;
    printf("\nNhập Index sản phẩm cần xem (bắt đầu từ 0): ");
    scanf("%d", &index);

    int fd = open("products.dat", O_RDONLY);
    if (fd < 0) {
        printf(" File dữ liệu products.dat chưa tồn tại.\n");
        return;
    }

    // Tính toán vị trí byte offset dựa trên công thức toán học
    off_t offset = (off_t)index * sizeof(Product);

    // Kiểm tra xem chỉ mục nhập vào có vượt quá kích thước file hiện tại không
    off_t file_size = lseek(fd, 0, SEEK_END);
    if (offset >= file_size || offset < 0) {
        printf(" Lỗi: Index vượt quá giới hạn file hoặc không hợp lệ!\n");
        close(fd);
        return;
    }

    // Nhảy con trỏ trực tiếp tới vị trí của bản ghi cần đọc
    lseek(fd, offset, SEEK_SET);

    Product p;
    if (read(fd, &p, sizeof(Product)) == sizeof(Product)) {
        printf("\n THÔNG TIN SẢN PHẨM TẠI INDEX [%d]:\n", index);
        printf(" ID      : %d\n", p.id);
        printf(" Tên     : %s\n", p.name);
        printf(" Số lượng: %d\n", p.quantity);
        printf(" Đơn giá : %.2f\n", p.price);
    }

    close(fd);
}

// Chức năng 3: Cập nhật DUY NHẤT trường quantity theo index
void update_quantity_by_index() {
    int index, new_qty;
    printf("\nNhập Index sản phẩm cần cập nhật số lượng: ");
    scanf("%d", &index);

    // Mở ở chế độ O_RDWR (Cả Đọc và Ghi) vì cần kiểm tra kích thước và cập nhật
    int fd = open("products.dat", O_RDWR);
    if (fd < 0) {
        printf(" File dữ liệu products.dat chưa tồn tại.\n");
        return;
    }

    // Tính toán offset của bản ghi và offset riêng biệt của trường quantity bên trong bản ghi đó
    off_t record_offset = (off_t)index * sizeof(Product);
    off_t field_offset = record_offset + offsetof(Product, quantity);

    off_t file_size = lseek(fd, 0, SEEK_END);
    if (record_offset >= file_size || record_offset < 0) {
        printf(" Lỗi: Index sản phẩm không tồn tại!\n");
        close(fd);
        return;
    }

    printf("Nhập số lượng mới: ");
    scanf("%d", &new_qty);

    // Dịch chuyển con trỏ file thẳng tới vị trí byte của trường quantity
    lseek(fd, field_offset, SEEK_SET);

    // Ghi đè duy nhất giá trị int (4 bytes) của biến new_qty, giữ nguyên toàn bộ dữ liệu khác
    if (write(fd, &new_qty, sizeof(int)) == sizeof(int)) {
        printf(" Thành công: Đã cập nhật riêng số lượng tại index %d trực tiếp trên ổ đĩa!\n", index);
    } else {
        perror("Lỗi cập nhật dữ liệu");
    }

    close(fd);
}

// Chức năng 4: Liệt kê toàn bộ danh sách sản phẩm (Đọc tuần tự)
void list_all_products() {
    int fd = open("products.dat", O_RDONLY);
    if (fd < 0) {
        printf("\n Danh sách trống (Chưa có file dữ liệu products.dat).\n");
        return;
    }

    Product p;
    int index = 0;
    printf("\n DANH SÁCH SẢN PHẨM \n");
    printf("%-7s %-10s %-30s %-12s %-10s\n", "Index", "ID", "Tên Sản Phẩm", "Số Lượng", "Đơn Giá");
    printf("------------------------------------------------------------------\n");

    while (read(fd, &p, sizeof(Product)) == sizeof(Product)) {
        printf("[%-5d] %-10d %-30s %-12d %-10.2f\n", index++, p.id, p.name, p.quantity, p.price);
    }

    close(fd);
}

int main() {
    int choice;
    while (1) {
        printf("\n QUẢN LÝ KHO (RANDOM ACCESS - LSEEK) \n");
        printf("1. Add product\n");
        printf("2. Show product by index\n");
        printf("3. Update quantity by index\n");
        printf("4. List all products\n");
        printf("5. Exit\n");
        printf("Nhập lựa chọn của bạn: ");
        
        if (scanf("%d", &choice) != 1) {
            printf("Lỗi định dạng đầu vào!\n");
            break;
        }

        switch (choice) {
            case 1: add_product(); break;
            case 2: show_product_by_index(); break;
            case 3: update_quantity_by_index(); break;
            case 4: list_all_products(); break;
            case 5:
                printf(" Đang thoát hệ thống.\n");
                exit(0);
            default:
                printf(" Lựa chọn sai, vui lòng nhập lại.\n");
        }
    }
    return 0;
}