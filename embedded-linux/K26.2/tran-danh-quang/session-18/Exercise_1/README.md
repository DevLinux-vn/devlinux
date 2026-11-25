# SSD1306 I2C character driver

Driver cho OLED SSD1306 128×64 tại địa chỉ 7-bit `0x3c`, cùng ứng dụng demo.
Driver giữ framebuffer 1024 byte trong kernel và xuất `/dev/ssd1306_i2c`.
Thiết kế dùng một màn hình; nhiều tiến trình có thể mở thiết bị đồng thời.

## Môi trường và build

Phần cứng theo đề: Raspberry Pi Zero W hoặc Pi 3B+, SDA GPIO2, SCL GPIO3,
OLED I2C 128×64 có charge pump nội, đã được reset đúng theo module phần cứng.
Kiểm tra điện áp và chân cấp nguồn theo module thực tế. Đặt bus I2C ở **100 kHz**
trong cấu hình controller của BSP/Device Tree (`clock-frequency = <100000>`).
Driver thiết bị không tự đổi tần số của controller dùng chung.

Mã sử dụng API **Linux 6.1**: `i2c_driver.probe_new`, `void remove`,
`class_create(THIS_MODULE, name)`. Đề chưa cung cấp revision BSP/Yocto SDK;
đối chiếu header của BSP trước khi build cho board, nhất là các API trên.

Trong shell đã nạp environment của Yocto SDK:

```bash
# KDIR phải là kernel build tree đã chuẩn bị, đúng config/Module.symvers của board.
# Ví dụ bên dưới dành cho image ARM 32-bit; chọn ARCH theo image thực tế.
make KDIR=/path/to/prepared/kernel-build ARCH=arm CC="$CC"
```

Kbuild nhận `ARCH`, `CROSS_COMPILE`, `CC` từ SDK hoặc tham số make. Makefile app
giữ nguyên `CC` được SDK export, kể cả tham số sysroot; nếu dùng toolchain riêng:

```bash
make KDIR=/path/to/prepared/kernel-build ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-
```

Makefile tại gốc `Exercise_1/` điều phối hai Makefile con. Khi truyền `KDIR`,
`make` build cả module và app; `make -j2` hỗ trợ build song song. Không có SDK hoặc
kernel build tree, `make` chỉ build app native và thông báo module chưa được build.
Có thể chọn riêng `make app` hoặc `make driver KDIR=/path/to/kernel-build`.
Driver không mặc định dùng kernel đang chạy. Module build trên x86-64 chỉ là kiểm
tra biên dịch, không dùng module đó trên Raspberry Pi.

## Khai báo thiết bị và chạy trên board

BSP cần bật `CONFIG_I2C`, controller I2C của board, `CONFIG_MODULES`,
`CONFIG_DEVTMPFS` và devtmpfs/udev để tạo node. Đảm bảo màn hình không được driver
khác giữ, ví dụ framebuffer SSD1306 có sẵn của kernel.

Trong node I2C của board, khai báo thiết bị tương đương:

```dts
clock-frequency = <100000>;
oled@3c {
    compatible = "tran-danh-quang,ssd1306";
    reg = <0x3c>;
};
```

Nếu BSP không khai báo DT cho màn hình, có thể tạo I2C client thủ công **trên
board đã xác nhận bus 1 đúng và địa chỉ 0x3c đang trống**, sau khi nạp module:

```bash
sudo insmod driver/ssd1306_i2c_driver.ko
echo ssd1306_char 0x3c | sudo tee /sys/bus/i2c/devices/i2c-1/new_device
sudo ./app/ssd1306_app
```

Nếu DT đã tạo client thì chỉ nạp module và chạy app, không dùng `new_device`.
Ứng dụng cần quyền đọc/ghi node; cấu hình quyền phù hợp hoặc chạy bằng sudo.
Demo CLEAR → upload bitmap viền → ba TEXT → ON → đợi 3 giây → OFF → đọc status.
SIGINT/SIGTERM được xử lý để đóng fd và thử OFF nếu app đã bật màn hình.
Sau OFF, status đúng là `Status: OFF, Last text: SSD1306 Test, Synced: yes`.

Chỉ khi đã tạo client thủ công ở trên, gỡ nó bằng:

```bash
echo 0x3c | sudo tee /sys/bus/i2c/devices/i2c-1/delete_device
sudo rmmod ssd1306_i2c_driver
```

## Giao thức

- Mỗi `write()` là một lệnh hoàn chỉnh: `CLEAR`, `ON`, `OFF`, hoặc `TEXT <text>`.
  Chấp nhận một LF cuối từ `echo`, từ chối NUL, lệnh lạ hoặc dữ liệu vượt giới hạn.
  Write 0 byte không làm gì. Ví dụ: `echo 'TEXT Hello World' | sudo tee /dev/ssd1306_i2c`.
- Font 5×7 hỗ trợ A–Z, a–z, 0–9 và `- . ! ?` cùng space. Mỗi ô rộng 6 pixel;
  tối đa 21 ký tự/dòng, 8 dòng, tổng chuỗi tối đa 168 byte. LF nội bộ xuống dòng.
  Text bắt đầu ở góc trái trên, OR điểm sáng lên bitmap hiện có; muốn thay text
  sạch thì CLEAR trước. Chuỗi không hợp lệ không đổi framebuffer hay gửi I2C.
- `read()` trả snapshot status, hỗ trợ partial read và EOF. Mở lại để đọc snapshot
  mới; write/ioctl thành công reset offset của cùng fd. Các lần mở có snapshot riêng.
  Nếu nhiều luồng dùng chung một fd, ứng dụng nên tuần tự hóa cả read/write/ioctl.
- Bitmap upload dùng ioctl cố định, không lẫn raw binary với parser lệnh text;
  xem [BITMAP_UPLOAD_GUIDE.md](BITMAP_UPLOAD_GUIDE.md).
- Wire format: command `00 <byte>`, data `40 <tối đa 31 byte>`. Tổng một giao dịch
  không quá 32 byte. Mỗi flush đặt lại window cột 0–127, page 0–7 trước khi gửi.
- Mỗi giao dịch hoàn chỉnh được mutex bảo vệ. Lỗi I2C/short transfer trả `-EIO`;
  NACK do không có thiết bị trả `-ENODEV`. Không cam kết rollback phần cứng đã
  nhận một phần framebuffer: status báo `Synced: no`; gửi lại bitmap hoặc CLEAR
  để đồng bộ toàn frame. Lỗi ON/OFF báo power `UNKNOWN` đến lệnh ON/OFF thành công.
- Khi unbind, driver chặn I/O mới, đợi thao tác I2C hiện tại kết thúc rồi bỏ client.
  FD còn mở giữ software state nhưng I/O sau detach trả `-ENODEV`; không giữ GPIO/
  bus resource vô hạn để chờ ứng dụng đóng fd. Không hỗ trợ forced module unload.

## Lựa chọn khi đề không thống nhất

Kernel dùng `i2c_master_send()`, không mở `/dev/i2c-1` từ kernel; đó là giao diện
user-space của `i2c-dev`. Kích thước text theo yêu cầu 21×8 thay vì phép chia sai
trong Design Hints. Thêm ioctl upload để đáp ứng tiêu đề, output bitmap và sản phẩm
`BITMAP_UPLOAD_GUIDE.md`. Dùng status thực tế OFF thay vì ON trong output mẫu sau OFF.

## Dọn build

```bash
make clean
```

## Tài liệu API

- [Linux 6.1: I2C client drivers](https://docs.kernel.org/6.1/i2c/writing-clients.html)
- [SSD1306 datasheet và charge pump application note](https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf)
