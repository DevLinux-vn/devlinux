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
