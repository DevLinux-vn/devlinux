#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

// Định nghĩa cấu trúc Học sinh đúng chuẩn kích thước cố định như đề bài
typedef struct {
    int   id;
    char  name[64];
    int   age;
    float gpa;
} Student;

// Chức năng 1: Thêm học sinh (Append vào cuối file nhị phân)
void add_student() {
    Student s;
    
    // Nhập dữ liệu từ bàn phím (stdin)
    printf("\n THÊM HỌC SINH MỚI \n");
    printf("Nhập ID học sinh (Số nguyên): ");
    scanf("%d", &s.id);
    getchar();
    
    printf("Nhập họ và tên học sinh: ");
    fgets(s.name, sizeof(s.name), stdin);
    s.name[strcspn(s.name, "\n")] = 0; // Loại bỏ ký tự xuống dòng do fgets tự thêm vào
    
    printf("Nhập tuổi: ");
    scanf("%d", &s.age);
    
    printf("Nhập điểm GPA: ");
    scanf("%f", &s.gpa);

    // Mở file ở chế độ Chỉ Ghi (O_WRONLY), Tạo mới nếu chưa có (O_CREAT), và Ghi nối đuôi (O_APPEND)
    // Quyền truy cập 0644 tương đương: Người sở hữu đọc/ghi, người khác chỉ đọc
    int fd = open("students.dat", O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd < 0) {
        perror("Lỗi: Không thể mở file để ghi dữ liệu");
        return;
    }

    // Ghi trực tiếp nguyên khối vùng nhớ struct xuống file dữ liệu
    if (write(fd, &s, sizeof(Student)) != sizeof(Student)) {
        perror("Lỗi: Ghi dữ liệu không hoàn chỉnh");
    } else {
        printf(" Thành công: Đã lưu thông tin học sinh vào file students.dat!\n");
    }

    // Luôn đóng file descriptor sau khi hoàn thành nhiệm vụ
    close(fd);
}

// Chức năng 2: Liệt kê toàn bộ học sinh (Đọc tuần tự từ đầu đến cuối file)
void list_students() {
    // Mở file ở chế độ Chỉ Đọc (O_RDONLY)
    int fd = open("students.dat", O_RDONLY);
    if (fd < 0) {
        // Nếu file chưa tồn tại thì ngầm hiểu danh sách đang trống
        printf("\n Danh sách trống (Chưa có file dữ liệu students.dat).\n");
        return;
    }

    Student s;
    printf("\n DANH SÁCH HỌC SINH \n");
    printf("%-10s %-30s %-10s %-10s\n", "ID", "Họ và Tên", "Tuổi", "GPA");
    printf("------------------------------------------------------------\n");

    // Vòng lặp đọc liên tục: Cứ mỗi lần đọc đúng kích thước của 1 Struct Student thì in ra màn hình
    // Khi hàm read trả về số 0, có nghĩa là đã chạm tới cuối file (EOF)
    while (read(fd, &s, sizeof(Student)) == sizeof(Student)) {
        printf("%-10d %-30s %-10d %-10.2f\n", s.id, s.name, s.age, s.gpa);
    }

    close(fd);
}

// Chức năng 3: Tìm kiếm học sinh theo ID
void find_student() {
    int target_id;
    printf("\nNhập ID học sinh cần tìm kiếm: ");
    scanf("%d", &target_id);

    int fd = open("students.dat", O_RDONLY);
    if (fd < 0) {
        printf(" Chưa có dữ liệu học sinh nào được khởi tạo.\n");
        return;
    }

    Student s;
    int found = 0;

    // Quét tuần tự từng bản ghi từ đầu file để so khớp ID
    while (read(fd, &s, sizeof(Student)) == sizeof(Student)) {
        if (s.id == target_id) {
            printf("\n ĐÃ TÌM THẤY HỌC SINH CÓ ID [%d]:\n", target_id);
            printf(" Họ và Tên: %s\n", s.name);
            printf(" Tuổi     : %d\n", s.age);
            printf(" Điểm GPA : %.2f\n", s.gpa);
            found = 1;
            break; // Tìm thấy rồi thì dừng vòng lặp ngay lập tức để tiết kiệm tài nguyên
        }
    }

    if (!found) {
        printf(" Không tìm thấy học sinh nào khớp với ID = %d\n", target_id);
    }

    close(fd);
}

// Chương trình điều hướng vòng lặp Menu chính
int main() {
    int choice;
    while (1) {
        printf("\n QUẢN LÝ HỌC SINH (SYSTEM CALLS) \n");
        printf("1. Add student\n");
        printf("2. List all students\n");
        printf("3. Find student by ID\n");
        printf("4. Exit\n");
        printf("Nhập lựa chọn của bạn (1-4): ");
        
        if (scanf("%d", &choice) != 1) {
            printf("Lỗi: Lựa chọn đầu vào phải là số!\n");
            break;
        }

        switch (choice) {
            case 1: add_student(); break;
            case 2: list_students(); break;
            case 3: find_student(); break;
            case 4:
                printf(" Đang thoát chương trình!\n");
                exit(0);
            default:
                printf(" Lựa chọn không hợp lệ, vui lòng chọn lại từ 1 đến 4.\n");
        }
    }
    return 0;
}