# Upload bitmap 128×64

Bitmap gồm **đúng 1024 byte**, không có header BMP/PNG, không có palette và không
phải chuỗi hex. Layout là **page-major**, trùng GDDRAM của SSD1306:

```text
byte_index = (y / 8) * 128 + x
bit_index  = y % 8
pixel bật: pixels[byte_index] |= 1 << bit_index
```

Tọa độ `(0,0)` là góc trái trên; page 0 chứa hàng 0–7, page 7 chứa hàng 56–63.
Byte 0 là cột 0 của page 0; bit thấp nhất là pixel trên cùng. Màn hình full trắng
là 1024 byte `0xff`; full đen là 1024 byte `0x00`.

## Tạo file mẫu

Chạy trên máy phát triển, tạo một bitmap viền:

```python
from pathlib import Path

pixels = bytearray(1024)
for y in range(64):
    for x in range(128):
        if x in (0, 127) or y in (0, 63):
            pixels[(y // 8) * 128 + x] |= 1 << (y % 8)
Path("border.bin").write_bytes(pixels)
```

Chuyển `border.bin` sang board và chạy:

```bash
sudo ./app/ssd1306_app --bitmap border.bin
```

App yêu cầu file thường đủ 1024 byte và không có byte dư **trước khi mở thiết bị**.
FIFO, thư mục và device file bị từ chối; app không chờ dữ liệu từ stream.
Demo CLEAR → upload file → chồng lần lượt ba chuỗi text demo tại góc trái trên →
ON → đợi 3 giây → OFF → status. Không có `--bitmap`, app tự tạo bitmap viền.

## Gọi ioctl từ ứng dụng khác

Include `driver/ssd1306.h`, mở `/dev/ssd1306_i2c` bằng `O_RDWR`, rồi truyền:

```c
struct ssd1306_bitmap bitmap; /* Điền đủ bitmap.pixels[1024] trước khi gọi. */
int ret = ioctl(fd, SSD1306_IOC_UPLOAD_BITMAP, &bitmap);
if (ret == -1)
    perror("upload bitmap");
```

Sau upload, dùng `write(fd, "ON", 2)` để bật hiển thị nếu cần; kiểm tra kết quả
write đầy đủ. Lệnh `TEXT ...` tiếp theo giữ bitmap và OR các pixel font lên trên.
Upload mới thay toàn framebuffer và xóa `Last text`; CLEAR cũng xóa cả hai.

Ioctl dùng `_IOW(0xb7, 0, struct ssd1306_bitmap)`, struct chỉ chứa mảng `__u8` nên
có cùng layout 32/64-bit. Truyền con trỏ tới struct, không truyền trực tiếp số byte
hay con trỏ cấp hai. Trả 0 nếu toàn frame được gửi thành công; ứng dụng nhận `-1`
và errno khi lỗi. Kích thước/mã ioctl thuộc nhóm này không đúng: `EINVAL`; con trỏ
user không hợp lệ: `EFAULT`; tháo thiết bị: `ENODEV`; lỗi bus/short transfer: `EIO`.

Copy từ user hoàn tất trước khi sửa state/gửi bus. Nếu I2C lỗi giữa chừng, software
giữ frame cũ nhưng phần cứng có thể đã nhận một phần frame mới: `Synced: no`.
Sau khi khắc phục bus, upload lại toàn bộ 1024 byte hoặc CLEAR; mỗi lần flush đều
reset addressing window. Việc build hoặc upload qua mock không xác nhận màn hình
thật đã hiển thị đúng; cần kiểm tra riêng trên board đích.
