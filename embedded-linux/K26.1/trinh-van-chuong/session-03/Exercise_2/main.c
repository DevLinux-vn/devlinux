#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>    // Định nghĩa cờ open()
#include <unistd.h>   // Định nghĩa các syscall cấp thấp
#include <stddef.h>   // Cần dùng cho macro offsetof

typedef struct {
    int    id;
    char   name[64];
    int    quantity;
    double price;
} Product;

void print_menu() {
    printf("\n--- PRODUCT MANAGEMENT (`lseek`) ---\n");
    printf("1. Add product\n");
    printf("2. Show product by index\n");
    printf("3. Update quantity by index\n");
    printf("4. List all products\n");
    printf("5. Exit\n");
    printf("Choose an option: ");
}

int main() {
    int fd;
    int choice;
    Product p;

    // Mở file nhị phân ở chế độ Đọc/Ghi. Tạo mới nếu chưa có.
    fd = open("products.dat", O_RDWR | O_CREAT, 0644);
    if (fd < 0) {
        perror("Không thể mở file products.dat");
        exit(EXIT_FAILURE);
    }

    while (1) {
        print_menu();
        if (scanf("%d", &choice) != 1) {
            printf("Lựa chọn không hợp lệ!\n");
            while (getchar() != '\n'); 
            continue;
        }

        if (choice == 1) {
            // --- CHỨC NĂNG 1: ADD PRODUCT ---
            printf("Nhập ID sản phẩm: ");
            scanf("%d", &p.id);
            printf("Nhập tên sản phẩm (tối đa 63 ký tự): ");
            scanf(" %63[^\n]", p.name);
            printf("Nhập số lượng: ");
            scanf("%d", &p.quantity);
            printf("Nhập giá: ");
            scanf("%lf", &p.price);

            // Nhảy về cuối file để append sản phẩm mới vào đuôi
            if (lseek(fd, 0, SEEK_END) < 0) {
                perror("lseek lỗi");
                continue;
            }

            if (write(fd, &p, sizeof(Product)) != sizeof(Product)) {
                perror("Ghi sản phẩm thất bại");
            } else {
                printf("Đã thêm sản phẩm thành công!\n");
            }

        } else if (choice == 2) {
            // --- CHỨC NĂNG 2: SHOW BY INDEX ---
            int index;
            printf("Nhập chỉ số index cần xem (bắt đầu từ 0): ");
            scanf("%d", &index);

            // Công thức: Tính vị trí byte bắt đầu của sản phẩm thứ [index]
            off_t offset = (off_t)index * sizeof(Product);

            // Nhảy cóc thẳng tới vị trí offset đó từ đầu file (SEEK_SET)
            if (lseek(fd, offset, SEEK_SET) < 0) {
                perror("lseek không tìm thấy vị trí index này");
                continue;
            }

            // Đọc đúng 1 bản ghi tại vị trí đó
            ssize_t bytes_read = read(fd, &p, sizeof(Product));
            if (bytes_read == sizeof(Product)) {
                printf("\n[Thông tin sản phẩm tại Index %d]:\n", index);
                printf("ID: %d | Tên: %s | Số lượng: %d | Giá: %.2f\n", p.id, p.name, p.quantity, p.price);
            } else {
                printf("Không tồn tại sản phẩm tại index %d (Vượt quá kích thước file)\n", index);
            }

        } else if (choice == 3) {
            // --- CHỨC NĂNG 3: UPDATE QUANTITY BY INDEX (Chỉ ghi đè 1 trường) ---
            int index, new_qty;
            printf("Nhập chỉ số index sản phẩm cần cập nhật số lượng: ");
            scanf("%d", &index);
            printf("Nhập số lượng mới: ");
            scanf("%d", &new_qty);

            // Bước A: Tính vị trí của sản phẩm đó trong file
            off_t offset = (off_t)index * sizeof(Product);
            
            // Bước B: Cộng thêm khoảng lệch (offset) của riêng biến `quantity` nằm trong struct
            off_t field_offset = offset + offsetof(Product, quantity);

            // Bước C: Nhảy cóc thẳng tới ô nhớ của biến `quantity` trên ổ đĩa
            if (lseek(fd, field_offset, SEEK_SET) < 0) {
                perror("lseek lỗi");
                continue;
            }

            // Bước D: Chỉ ghi đè đúng dung lượng của một số nguyên (int), không ghi cả struct!
            ssize_t bytes_written = write(fd, &new_qty, sizeof(int));
            if (bytes_written == sizeof(int)) {
                printf("Đã cập nhật số lượng cho sản phẩm tại index %d thành công!\n", index);
            } else {
                printf("Cập nhật thất bại. Index không tồn tại.\n");
            }

        } else if (choice == 4) {
            // --- CHỨC NĂNG 4: LIST ALL ---
            if (lseek(fd, 0, SEEK_SET) < 0) {
                perror("lseek lỗi");
                continue;
            }

            printf("\nDanh sách toàn bộ sản phẩm:\n");
            printf("%-5s | %-5s | %-20s | %-10s | %-10s\n", "Idx", "ID", "Tên sản phẩm", "Số lượng", "Giá");
            printf("----------------------------------------------------------------------\n");

            int idx = 0;
            while (read(fd, &p, sizeof(Product)) == sizeof(Product)) {
                printf("%-5d | %-5d | %-20s | %-10d | %-10.2f\n", idx, p.id, p.name, p.quantity, p.price);
                idx++;
            }

        } else if (choice == 5) {
            // --- CHỨC NĂNG 5: EXIT ---
            printf("Đang đóng chương trình...\n");
            break;
        }
    }

    close(fd);
    return 0;
}