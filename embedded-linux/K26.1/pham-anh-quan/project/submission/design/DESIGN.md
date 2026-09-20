# DESIGN.md — Project 3: Multi-user CLI Chat Server/Client

**Học viên:** Phạm Anh Quân | **Ngày nộp:** 20/09/2026

---

## 1. Kiến trúc tổng thể

Sơ đồ luồng xử lý của server dựa trên mô hình **Thread-per-client** (Mỗi client 1 thread) kết hợp Blocking I/O & Mutex Synchronization:
+-------------------------+
                   |    Main Listening Loop  |
                   |  socket() -> bind()     |
                   |       -> listen()       |
                   +------------+------------+
                                |
                         accept() (blocking)
                                |
                                v  (Client kết nối mới)
                   +-------------------------+
                   |     pthread_create()    |
                   +------------+------------+
                                |
                +---------------+---------------+
                |                               |
                v                               v
     +--------------------+           +--------------------+
     |   Client Thread 1  |           |   Client Thread 2  |
     |  client_handler()  |           |  client_handler()  |
     +---------+----------+           +---------+----------+
               |                                |
         recv() (blocking)                recv() (blocking)
               |                                |
               v                                v
+------------------------------+ +------------------------------+
| Mutex Lock (clients_mutex)   | | Mutex Lock (clients_mutex)   |
| - Đăng nhập / Đăng ký        | | - Đăng nhập / Đăng ký        |
| - Broadcast tin nhắn         | | - Broadcast tin nhắn         |
| - Xử lý /who                 | | - Xử lý /who                 |
| - Ghi history (dùng flock)   | | - Ghi history (dùng flock)   |
| Mutex Unlock                 | | Mutex Unlock                 |
+------------------------------+ +------------------------------+


---

## 2. Danh sách Process/Thread

| Tên | Loại (process/thread) | Vai trò | Tạo lúc nào | Kết thúc lúc nào | Giao tiếp với ai — qua kênh gì |
|---|---|---|---|---|---|
| `main` | process (main thread) | Khởi tạo socket server, lắng nghe kết nối (`accept()`), cấp phát ô nhớ trong `clients[]` và tạo thread mới. | Khởi động server | Khi tắt server (`SIGINT`/`Ctrl+C`) | Client mới — qua Socket TCP (`accept()`) |
| `client_handler` | thread (`pthread`) | Xử lý nhận/gửi dữ liệu (`recv`/`send`) cho 1 client riêng biệt; xử lý Authen, Chat, Command. | Được `main` thread tạo qua `pthread_create()` ngay khi có client kết nối. | Khi client ngắt kết nối, thoát chương trình hoặc gửi sai pass 3 lần. | Với client tương ứng qua Socket TCP; Với các client khác qua mảng shared `clients[]` được bảo vệ bởi `clients_mutex`. |

> **Giải thích kiến trúc:** Hệ thống tuân thủ yêu cầu **Thread-per-client model** (không dùng non-blocking epoll). Main thread chỉ làm nhiệm vụ chấp nhận kết nối. Mỗi client được phục vụ bởi 1 POSIX Thread (`pthread`) chạy vòng lặp `recv()` blocking độc lập. Việc truy cập vào tài nguyên dùng chung (danh sách client, file tài khoản, lịch sử chat) được đồng bộ hóa hoàn toàn bằng `pthread_mutex_t` và `flock()`.

---

## 3. Định nghĩa Protocol

Hệ thống sử dụng **Text-based protocol dạng dòng lệnh (IRC-like)**, kết thúc mỗi message/gói tin bằng ký tự chuyển dòng `\n`.

Liệt kê chi tiết toàn bộ các loại message giữa Client ↔ Server:

| Chiều | Loại message | Định dạng | Ví dụ |
|---|---|---|---|
| C→S | Đăng nhập | `LOGIN <username> <password>\n` | `LOGIN quan 123456\n` |
| C→S | Đăng ký | `REGISTER <username> <password>\n` | `REGISTER quan 123456\n` |
| C→S | Gửi tin nhắn chat | `<nội_dung_tin_nhắn>\n` | `Hello mọi người!\n` |
| C→S | Lấy danh sách online | `/who\n` | `/who\n` |
| C→S | Thoát ứng dụng | `/quit\n` | `/quit\n` |
| S→C | Phản hồi Authen thành công | `OK <thông_báo>\n` | `OK Login successful\n` |
| S→C | Phản hồi Authen thất bại | `ERR <thông_báo_lỗi>\n` | `ERR Invalid username or password\n` |
| S→C | Broadcast tin nhắn chat | `MSG <username>: <nội_dung>\n` | `MSG quan: Hello mọi người!\n` |
| S→C | Lịch sử tin nhắn | `HIST [<timestamp>] <username>: <nội_dung>\n` | `HIST [10:32:01] minh: chào mọi người\n` |
| S→C | Danh sách user online | `ONLINE_USERS: <user1>, <user2>...\n` | `ONLINE_USERS: quan, minh, lan\n` |
| S→C | Thông báo hệ thống | `SYS <thông_báo>\n` | `SYS *** quan has joined ***\n` |

---

## 4. Xử lý Socket Blocking & Buffer Message

- **Cấu trúc lưu trữ Client:**
  ```c
  typedef struct {
      int fd;
      char username[64];
      int authenticated;
      int failed_attempts; // Đếm số lần nhập sai pass
      pthread_t thread_id;
  } client_t;

  Xử lý Message Delimiter (\n): Dù dùng blocking socket, recv() có thể nhận một phần gói tin hoặc gộp nhiều dòng. Mỗi client_handler duy trì một buffer đọc cá nhân (char rx_buf[4096]). Sau mỗi lần recv(), thread tiến hành nối chuỗi và tìm ký tự \n để tách thành các message hoàn chỉnh.

Đồng bộ Gửi (send): Do các thread có thể gọi send() đến cùng một fd của client (ví dụ khi Broadcast), hàm gửi dữ liệu send_bytes() được bọc trong critical section hoặc ghi trực tiếp với mutex bảo vệ mảng client để tránh tình trạng đè dữ liệu.

5. Quản lý State & Dữ liệu
Quản lý danh sách Client Online:

Dùng mảng cố định client_t clients[MAX_CLIENTS].

Mọi thao tác tìm slot trống, thêm user, duyệt danh sách để broadcast hoặc lấy danh sách /who đều bắt buộc phải pthread_mutex_lock(&clients_mutex) trước khi thực hiện.

Quản lý Tài khoản (Auth):

Lưu tại file users.txt.

Mật khẩu không lưu dạng plaintext mà lưu dạng Hash bằng hàm crypt() (glibc) với Salt.

Định dạng file users.txt: <username>:<hashed_password>

Quản lý Lịch sử Tin nhắn:

Lưu tại file chat_history.txt dạng append-only.

Sử dụng file lock flock(fd, LOCK_EX) khi ghi tin nhắn mới.

Khi user đăng nhập thành công, server dùng flock(fd, LOCK_SH) để đọc toàn bộ lịch sử file và truyền dòng HIST ... về client trước khi thông báo join phòng realtime.
6. Xử lý client ngắt kết nối đột ngột
Phát hiện disconnect: Trong thread client_handler, hàm recv() trả về:

= 0: Client đã chủ động ngắt kết nối (Close socket).

< 0: Có lỗi socket hoặc client bị kill đột ngột (kill -9, rớt mạng).

Quy trình Cleanup (Giải phóng tài nguyên):

Đóng descriptor close(client->fd).

Khóa pthread_mutex_lock(&clients_mutex).

Đặt client->fd = -1, client->authenticated = 0, xóa username về rỗng.

Thông báo System Broadcast cho các client khác: SYS *** <username> has left ***.

Giải phóng clients_mutex.

Thread tự kết thúc sạch sẽ bằng return NULL hoặc pthread_exit(NULL).
## 7. Edge Case / Failure Handling dự kiến

| Tình huống | Cách xử lý dự kiến |
|---|---|
| Client gửi message không đúng protocol | Server gửi lại `ERR Invalid command format\n`, bỏ qua gói tin lỗi và tiếp tục duy trì kết nối. |
| Client kill đột ngột (`kill -9`) giữa lúc đang gõ dở | `recv()` trả về `<= 0`, thread nhảy vào luồng Cleanup, đóng fd và giải phóng slot mà không crash server. |
| Nhập sai username/password 3 lần liên tiếp | Server phản hồi chung `ERR Invalid username or password`, tăng `failed_attempts`. Sau 3 lần sai, server đóng kết nối `close(fd)`. |
| File tài khoản / Lịch sử bị khóa bởi tiến trình/thread khác | Sử dụng `flock()` chờ theo cơ chế blocking hợp lý, bảo vệ tính toàn vẹn dữ liệu khi ghi đồng thời. |
| Nhiều client đăng nhập đồng thời cùng 1 tài khoản | Khóa mutex kiểm tra mảng `clients[]`, nếu tài khoản đã ở trạng thái `authenticated == 1` thì từ chối đăng nhập mới và trả về `ERR Account already logged in`. |

---

## 8. Bảng đối chiếu Requirement Coverage

| Requirement ID | Mô tả ngắn | Đề cập ở mục | Ghi chú |
|---|---|---|---|
| **P3-M1** | Đăng ký/đăng nhập, mật khẩu hash | Mục 3, 5 | Dùng `crypt()` với Salt, lưu `users.txt` |
| **P3-M2** | Broadcast 1 phòng chung | Mục 1, 3, 4 | Broadcast qua mảng `clients[]` có mutex |
| **P3-M3** | Thread-per-client model & Mutex sync | Mục 1, 2, 4 | Dùng `pthread_create()` + `pthread_mutex_t` |
| **P3-M4** | Xử lý disconnect đột ngột | Mục 6, 7 | Cleanup tài nguyên, đóng fd sạch sẽ |
| **P3-M5** | Danh sách user online (`/who`) | Mục 3, 5 | Lấy thông tin từ mảng `clients[]` |
| **P3-M6** | Xử lý sai username/pass 3 lần | Mục 3, 7 | Không báo cụ thể lỗi sai user hay pass, ngắt sau 3 lần |
| **P3-M7** | Lưu & gửi lại lịch sử tin nhắn khi join | Mục 3, 5 | Dùng `flock()` đọc/ghi `chat_history.txt` |
9. Rủi ro thấy khó nhất trong project này
Race Condition khi Broadcast & Disconnect đồng thời: Nếu một client ngắt kết nối trong lúc thread khác đang duyệt mảng clients[] để gửi tin nhắn broadcast. Giải pháp là bắt buộc khóa clients_mutex trong suốt quá trình broadcast và kiểm tra tính hợp lệ fd >= 0 && authenticated == 1 trước khi gọi send().

Xung đột ghi File History đồng thời: Khi nhiều client gửi tin nhắn cùng một thời điểm, các thread ghi vào file chat_history.txt có thể làm hỏng nội dung. Giải pháp là sử dụng flock(fd, LOCK_EX) để đảm bảo ghi nguyên tử (atomic write).
