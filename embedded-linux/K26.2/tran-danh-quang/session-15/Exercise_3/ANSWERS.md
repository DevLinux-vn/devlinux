# Session 15 — Trả lời lý thuyết

## 1. Vì sao phải dùng copy_from_user()/copy_to_user()?

- Con trỏ user-space là địa chỉ không đáng tin cậy: có thể NULL, ngoài vùng
  user-space, trỏ vào trang chưa ánh xạ hoặc thiếu quyền đọc/ghi. `memcpy()` hay
  dereference trực tiếp không cung cấp cơ chế uaccess để kiểm tra và phục hồi
  lỗi truy cập; lỗi có thể trở thành kernel oops thay vì trả `-EFAULT` cho tiến
  trình. Các cơ chế bảo vệ như SMAP/PAN còn hạn chế truy cập user memory trực
  tiếp từ kernel.
- Trang user-space có thể chưa resident, gây page fault và cần ngủ để xử lý.
  `copy_from_user()`/`copy_to_user()` dùng đường truy cập được kernel hỗ trợ,
  có exception fixup; vẫn phải gọi trong ngữ cảnh cho phép ngủ, không giữ
  spinlock. Chúng trả **số byte chưa copy**, không phải mã errno và cũng không
  phải số byte đã copy.

Chỉ chép giá trị con trỏ vào buffer không chép dữ liệu mà con trỏ trỏ tới.
Hai driver giới hạn kích thước trước khi copy, dùng buffer kernel riêng và
không giữ lại con trỏ user. Nếu `copy_from_user()` thất bại dù chỉ một phần,
write trả `-EFAULT` và không thay đổi dữ liệu dùng chung. Read dùng
`simple_read_from_buffer()`, helper xử lý copy ra user, cập nhật offset theo
số byte thực sự copy; nếu không copy được byte nào do lỗi thì trả `-EFAULT`.

## 2. Vai trò của *ppos

`*ppos` là vị trí byte hiện tại của lần đọc/ghi đối với open file description.
Mỗi lần mở riêng có offset riêng; các fd từ `dup()` hoặc kế thừa qua `fork()`
có thể dùng chung open file description. Với buffer dài N byte, read chỉ trả
phần từ offset tới hết buffer, giới hạn bởi số byte user yêu cầu, rồi tăng
offset theo số byte thực sự trả. Khi offset >= N, read trả 0 (EOF).

Nếu không cập nhật offset, các lần read có thể trả lặp lại dữ liệu đầu buffer;
`cat` có thể đọc mãi vì không gặp EOF. Hai driver dùng
`simple_read_from_buffer()` để hỗ trợ partial read, EOF và offset âm. Sau một
write hợp lệ, driver reset `*ppos = 0` cho chính open file description đó để
đọc lại giá trị mới. Không reset offset của các lần mở độc lập khác.

Dữ liệu là giá trị dùng chung mới nhất tại mỗi lần read, không phải snapshot
cố định cho cả chuỗi read. Mutex ngăn read thấy buffer đang cập nhật dở;
một write xen giữa hai lần read vẫn có thể đổi nội dung/độ dài mà lần read sau
nhìn thấy. Mở lại thiết bị để bắt đầu đọc từ đầu. Thiết bị không hỗ trợ seek
(`nonseekable_open()`).

## 3. alloc_chrdev_region() và register_chrdev()

`alloc_chrdev_region(&dev, first_minor, count, name)` cấp một dải device number
với major được chọn động và số minor cần dùng. Nó chỉ đăng ký dải số, chưa
gắn file operations. Driver tiếp tục `cdev_init()` và `cdev_add()` để đăng ký
đối tượng `cdev`, rồi `class_create()`/`device_create()` để xuất thiết bị qua
device model. Devtmpfs/udev trên hệ thống đích tạo node `/dev` tương ứng.

`register_chrdev()` là giao diện cũ gộp cấp số và đăng ký cdev, dành một dải
256 minor cho major đó; major bằng 0 vẫn có thể được cấp động. Vì thế ưu điểm
của cách mới không đơn thuần là major động: nó cho phép chọn đúng số minor,
quản lý từng cdev/file operations rõ ràng và ghép vòng đời thiết bị linh hoạt.
Trong bài, mỗi driver chỉ xin một minor và hoàn trả bằng
`unregister_chrdev_region()`. `class_create()` và `device_create()` vẫn cần
thiết ngay cả khi dùng giao diện cũ nếu muốn xuất thiết bị qua device model.

## 4. Hai tiến trình cùng ghi "inc" có gây race không?

Nếu chỉ dùng `counter_value++` mà không đồng bộ thì có. Phép tăng là chuỗi
đọc–sửa–ghi: cả hai CPU có thể cùng đọc 5 rồi cùng ghi 6, làm mất một lần
tăng; kết quả đúng phải là 7. `volatile` không làm chuỗi này atomic.

Trong `counter_chardev.c` của bài này, tất cả truy cập bộ đếm được bảo vệ bằng
cùng `counter_lock`. Hai write phải lần lượt lấy mutex nên mỗi lần tăng đều
thấy kết quả của lần trước. Cả kiểm tra `INT_MAX` và phép tăng nằm trong cùng
critical section, trả `-EOVERFLOW` nếu tăng sẽ tràn; `dec` tại 0 giữ nguyên 0.
Read cũng dùng mutex để lấy dữ liệu nhất quán trong một lần gọi. Mutex phù
hợp với file operations ở process context, nơi thao tác usercopy có thể ngủ.

## Ghi chú phạm vi và lựa chọn

- Cả ba Exercise là `[review-only]`; sản phẩm theo đúng tên file trong đề,
  không kèm Makefile và không yêu cầu nạp module. Chữ ký `void module_exit`
  là bắt buộc theo API kernel, dù quy tắc chung của workspace ưu tiên hàm
  trả trạng thái.
- Session không chỉ định kernel, kiến trúc, kernel build tree hay toolchain.
  Linux upstream 6.1 và 6.4 được dùng để đối chiếu API, không được suy ra là
  môi trường đích của lớp. `class_create()` dùng hai tham số trước 6.4 và một
  tham số từ 6.4; kernel vendor có backport cần đối chiếu header riêng.
- Counter nhận đúng một lệnh hoàn chỉnh mỗi write, không phân biệt hoa/thường,
  cho phép một LF cuối để tương thích `echo`; từ chối tiền tố như `incXYZ`,
  NUL nhúng và lệnh không hợp lệ với `-EINVAL`. Write 0 byte là no-op.
- Reverse nhận tối đa 255 **byte**, đảo byte (không giải mã Unicode), giữ cả
  newline/NUL; write quá dài trả `-EMSGSIZE`, không cắt ngầm. Write 0 byte
  không xóa dữ liệu. Đây là cách diễn giải theo buffer `char[256]` của đề.
- `.owner = THIS_MODULE` giữ module khi còn file đang mở/thao tác. Cờ ready
  từ chối open trước khi khởi tạo hoàn tất; đường lỗi thu hồi đúng những tài
  nguyên đã tạo, theo thứ tự ngược. Normal unload không chạy khi còn module
  users; không hỗ trợ forced unload. Không có công việc bất đồng bộ/timer.
- Không đọc lại driver mẫu hoặc lời giải session khác; câu trả lời dựa trên
  đề session này, hai driver tự viết và nguồn Linux chính thức bên dưới.

## Nguồn đối chiếu

- [Linux 6.1: class_create(owner, name)](https://raw.githubusercontent.com/torvalds/linux/v6.1/include/linux/device/class.h)
- [Linux 6.4: class_create(name)](https://github.com/torvalds/linux/blob/v6.4/include/linux/device/class.h)
- [Linux 6.1 Kernel API: cdev và device numbers](https://docs.kernel.org/6.1/core-api/kernel-api.html)
- [Linux 6.1: user-space memory access](https://docs.kernel.org/6.1/core-api/mm-api.html#user-space-memory-access)
- [Linux: escaped buffers trong printk](https://docs.kernel.org/core-api/printk-formats.html#raw-buffer-as-an-escaped-string)
- [Linux 6.1: simple_read_from_buffer()](https://github.com/torvalds/linux/blob/v6.1/fs/libfs.c)
- [Linux 6.1: cdev publication và module references](https://github.com/torvalds/linux/blob/v6.1/fs/char_dev.c)
