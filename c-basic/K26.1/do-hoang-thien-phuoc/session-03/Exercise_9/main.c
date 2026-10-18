#include <stdio.h>

int main() {
    int loai_khach, loai_ngay;
    double gia_ve = 10000; // Giá vé cơ bản

    // Nhập loại hành khách
    printf("Nhap loai hanh khach (1: Tre em, 2: Nguoi lon, 3: Nguoi gia): ");
    scanf("%d", &loai_khach);

    // Nhập loại ngày
    printf("Nhap loai ngay (1: Ngay thuong, 2: Ngay le): ");
    scanf("%d", &loai_ngay);

    // Xử lý tăng giá ngày lễ trước
    if (loai_ngay == 2) {
        gia_ve = gia_ve + (gia_ve * 0.5); // Tăng 50%
    } else if (loai_ngay != 1) {
        printf("Loi: Loai ngay khong hop le!\n");
        return 1; // Thoát chương trình nếu nhập sai ngày
    }

    // Xử lý giảm giá theo loại hành khách trên mức giá hiện tại
    if (loai_khach == 1) {
        gia_ve = gia_ve - (gia_ve * 0.3); // Trẻ em giảm 30%
    } else if (loai_khach == 3) {
        gia_ve = gia_ve - (gia_ve * 0.2); // Người già giảm 20%
    } else if (loai_khach != 2) {
        printf("Loi: Loai hanh khach khong hop le!\n");
        return 1; // Thoát chương trình nếu nhập sai khách
    }
    // (Người lớn = 2 không được giảm nên không cần viết lệnh tính lại giá)

    // In ra số tiền cuối cùng (ép kiểu .0lf để in ra số nguyên, không lấy phần thập phân)
    printf("So tien cuoi cung: %.0lf d\n", gia_ve);

    return 0;
}