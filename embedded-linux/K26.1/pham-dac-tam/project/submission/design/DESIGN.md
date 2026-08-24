# DESIGN.md — Project 4: Infrastructure Health Monitoring Agent

**Học viên:** Phạm Đắc Tâm  |  **Ngày nộp:** ____________________

> File này được nộp trước khi mở PR code. Trọng tâm review: protocol agent↔server, cơ chế phát hiện OFFLINE, và thiết kế dashboard 2 phía (agent + server).

---

## 1. Kiến trúc tổng thể

Hệ thống gồm 2 thành phần độc lập:
- Agent: chạy trên từng máy khách, đọc chỉ số hệ thống từ /proc và /sys, gửi dữ liệu định kỳ về server qua TCP, đồng thời tự vẽ dashboard riêng trên terminal.
- Server: lắng nghe nhiều kết nối agent đồng thời bằng epoll, lưu trạng thái từng agent, ghi log có cấu trúc, phát hiện OFFLINE, và vẽ dashboard tổng hợp trên CLI.

Sơ đồ luồng:

```text
+-------------------+        TCP        +---------------------------+
|   Agent           | <-------------> |   Server                  |
| - đọc /proc       |                 | - epoll loop              |
| - thu thập dữ liệu|                 | - dashboard tổng hợp      |
| - gửi heartbeat   |                 | - log dữ liệu/alert       |
| - dashboard riêng |                 | - /config /history        |
+-------------------+                 +---------------------------+
```

---

## 2. Danh sách Process/Thread

### Phía Agent

| Tên | Loại | Vai trò | Tạo lúc nào | Kết thúc lúc nào | Giao tiếp với ai — qua kênh gì |
|---|---|---|---|---|---|
| main | process | khởi động agent, tạo thread và kết nối tới server | khi chạy chương trình | khi agent dừng | kết nối TCP tới server |
| collector_thread | thread | đọc /proc, thu thập CPU/RAM/DISK/NET, gửi dữ liệu định kỳ | sau khi kết nối TCP thành công | khi agent thoát | gửi dữ liệu qua socket TCP |
| dashboard_thread | thread | vẽ dashboard riêng của agent trên terminal | sau khi agent khởi động | khi agent thoát | chia sẻ struct dữ liệu qua mutex |
| monitor_thread (nếu cần) | thread | theo dõi trạng thái kết nối, retry nếu socket bị lỗi | khi agent khởi động | khi agent thoát | giao tiếp nội bộ qua biến/chung |

### Phía Server

| Tên | Loại | Vai trò | Tạo lúc nào | Kết thúc lúc nào | Giao tiếp với ai — qua kênh gì |
|---|---|---|---|---|---|
| main / epoll_loop | process | lắng nghe kết nối mới, xử lý dữ liệu từ agent, xử lý stdin command | khi server khởi động | khi server dừng | nhận từ socket agent và stdin |
| dashboard_refresh_loop | logic trong epoll loop | refresh dashboard mỗi 2 giây, kiểm tra timeout OFFLINE | khi server chạy | khi server dừng | dùng timerfd + map trạng thái |
| command_handler | logic trong epoll loop | đọc lệnh /config, /history từ admin | khi server chạy | khi server dừng | qua STDIN_FILENO |

---

## 3. Định nghĩa Protocol Agent ↔ Server

Em chọn giao thức dạng JSON-line vì dễ debug, dễ mở rộng, và phù hợp cho việc parse bằng tay hoặc thư viện nhẹ.

| Chiều | Loại message | Định dạng | Ví dụ |
|---|---|---|---|
| Agent → Server | Dữ liệu định kỳ | `{"type":"data","agent_id":"web-01","cpu":52,"ram":81,"disk":30,"timestamp":...}` | `{"type":"data","agent_id":"web-01","cpu":52,"ram":81,"disk":30}` |
| Agent → Server | Heartbeat | `{"type":"heartbeat","agent_id":"web-01"}` | `{"type":"heartbeat","agent_id":"web-01"}` |
| Server → Agent | Config mới | `{"type":"config","key":"interval","value":5}` | `{"type":"config","key":"interval","value":5}` |
| Server → Agent | Ack / response | `{"type":"ack","status":"ok"}` | `{"type":"ack","status":"ok"}` |

Mỗi message kết thúc bằng `\n` để dễ đọc theo dòng. Server và agent dùng cùng 1 parser tối giản để tách field.

---

## 4. agent_id — sinh & lưu trữ

- Agent_id được sinh 1 lần duy nhất khi file cấu hình chưa tồn tại.
- Định dạng đề xuất: `host-<random>` hoặc `agent-<uuid7-like>`.
- File lưu trữ: `agent.id` ở thư mục làm việc hiện tại của agent, hoặc `./config/agent.id` nếu muốn tách riêng.
- Luồng hoạt động:
  1. Nếu file `agent.id` tồn tại → đọc và dùng lại giá trị cũ.
  2. Nếu không tồn tại → sinh giá trị mới, ghi file và dùng luôn.
- Điều này đảm bảo agent_id không đổi qua các lần restart, tránh server hiểu nhầm là agent mới.

---

## 5. Cơ chế đọc chỉ số hệ thống

### CPU
- Đọc `/proc/stat` dòng đầu tiên `cpu ...`.
- Lấy các cột: `user`, `nice`, `system`, `idle`, `iowait`, `irq`, `softirq`.
- Đọc 2 lần cách nhau 1 giây.
- Công thức:
  - `idle_time = idle + iowait`
  - `total_time = user + nice + system + idle + iowait + irq + softirq`
  - `cpu_usage = (1 - delta_idle / delta_total) * 100`

### RAM
- Đọc `/proc/meminfo`.
- Lấy `MemTotal` và `MemAvailable`.
- Công thức:
  - `ram_usage = (1 - MemAvailable / MemTotal) * 100`

### Disk
- Dùng `statvfs("/", &st)`.
- Tính theo công thức:
  - `disk_usage = ((f_blocks - f_bfree) / f_blocks) * 100`

### Network (tuỳ chọn)
- Đọc `/proc/net/dev` để lấy bytes RX/TX, có thể dùng cho dashboard mở rộng.

---

## 6. Cơ chế Keepalive 2 lớp & phát hiện OFFLINE

### Lớp 1: Heartbeat ứng dụng
- Agent gửi heartbeat mỗi 3 giây.
- Server cập nhật `last_heartbeat_time` mỗi khi nhận được `data` hoặc `heartbeat`.

### Lớp 2: TCP keepalive
- Bật `SO_KEEPALIVE` trên socket.
- Giá trị đề xuất:
  - `TCP_KEEPIDLE = 5s`
  - `TCP_KEEPINTVL = 2s`
  - `TCP_KEEPCNT = 3`
- Mục đích: phát hiện socket chết ở tầng TCP khi không có dữ liệu giao tiếp.

### OFFLINE detection
- Mỗi 2 giây, server kiểm tra thời gian từ `last_heartbeat_time` đến hiện tại.
- Nếu vượt ngưỡng timeout (ví dụ 9 giây hoặc 3× heartbeat interval) thì đổi trạng thái agent sang `OFFLINE`.
- Cách này tránh false positive khi chỉ mất một vài gói tạm thời.

---

## 7. Thiết kế Dashboard (Agent + Server)

### Mục tiêu
- Agent có dashboard riêng để quan sát chỉ số của chính nó.
- Server có dashboard tổng hợp cho tất cả agent đang kết nối hoặc từng kết nối trước đó.

### Hàm dùng chung vẽ thanh load
- Tách thành hàm `render_bar(percent, width)`.
- Input: `percent`, `label`, `metric_type`, `status`.
- Output: chuỗi đã tô màu và có thanh load dạng `█` / `░`.
- Quy tắc màu:
  - dưới WARNING → xanh
  - WARNING → vàng
  - CRITICAL → đỏ
  - nếu trạng thái là OFFLINE → đỏ cố định

### Cách server quản lý dashboard
- Server giữ 1 map dạng `agent_id -> agent_entry`.
- Mỗi entry lưu:
  - `agent_id`
  - `last_data`
  - `status` (`ONLINE` hoặc `OFFLINE`)
  - `last_heartbeat_time`
  - `last_seen`
- Dashboard server chỉ duyệt qua map này để vẽ đúng số dòng tương ứng với số agent đã từng kết nối hoặc đang kết nối.
- Khi agent mới kết nối, server thêm entry mới vào map.
- Khi agent OFFLINE, server đổi trạng thái mà không xoá dòng khỏi dashboard.

### Tái vẽ màn hình
- Dùng `\033[2J\033[H` để xoá và vẽ lại toàn bộ.
- Refresh mỗi 2 giây để tránh hiện tượng chồng dòng.

---

## 8. Thiết kế lệnh /command trên Server

Server cho phép admin nhập các lệnh sau trên terminal:

| Lệnh | Cú pháp | Hành vi |
|---|---|---|
| `/config` | `/config <agent_id> <key>=<value>` | Gửi config mới xuống đúng agent qua socket đang mở |
| `/history` | `/history <agent_id> [--last N]` | Đọc lại dữ liệu/ALERT log đã ghi từ file log của agent đó |
| `/help` | `/help` | Hiển thị danh sách lệnh hỗ trợ |

Ví dụ:
- `/config web-01 interval=5`
- `/history web-01 --last 10`

---

## 9. Thiết kế Logging

Server ghi 3 loại log riêng biệt để dễ phân tích và truy vấn.

### 1) Log dữ liệu định kỳ
- File: `logs/periodic.log`
- Mỗi dòng là 1 bản ghi dữ liệu máy theo format parseable, ví dụ:
  - `{"ts":"...","agent_id":"web-01","cpu":52,"ram":81,"disk":30}`

### 2) Log ALERT
- File: `logs/alert.log`
- Dùng khi một chỉ số vượt ngưỡng CRITICAL.
- Ví dụ:
  - `{"ts":"...","agent_id":"web-01","metric":"RAM","value":95,"threshold":"critical"}`

### 3) Log kết nối / ngắt kết nối / OFFLINE
- File: `logs/events.log`
- Ghi các sự kiện: connect, disconnect, offline, online lại.

Cách parse lại:
- Dùng JSON-line nên có thể đọc bằng các parser đơn giản.
- `/history` chỉ cần đọc file log tương ứng và lọc theo `agent_id` và `timestamp`.

---

## 10. Ngưỡng cảnh báo & màu sắc

Áp dụng đúng quy tắc thống nhất cho cả agent và server.

| Chỉ số | WARNING | CRITICAL |
|---|---:|---:|
| CPU | ≥ 70% | ≥ 90% |
| RAM | ≥ 75% | ≥ 90% |
| DISK | ≥ 80% | ≥ 95% |

| Mức | Màu hiển thị | ANSI |
|---|---|---|
| Bình thường | Xanh lá | `\033[32m` |
| Cảnh báo | Vàng | `\033[33m` |
| Nguy hiểm | Đỏ | `\033[31m` |
| Reset | — | `\033[0m` |

Quy tắc áp dụng:
- Nếu vượt CRITICAL thì ưu tiên màu đỏ.
- Thanh load và text phải cùng màu.
- Trạng thái OFFLINE trên dashboard server luôn hiển thị đỏ.

---

## 11. Edge Case / Failure Handling dự kiến

| Tình huống | Cách xử lý dự kiến |
|---|---|
| Agent mất kết nối tạm thời rồi kết nối lại | Giữ entry cũ, cập nhật trạng thái ONLINE lại, ghi sự kiện reconnect |
| Server nhận `/config` cho agent_id không tồn tại | Trả về thông báo lỗi `[ERR] agent_id not found` |
| Agent restart nhưng agent_id trùng với agent đang ONLINE khác | Giữ lại agent_id cũ, server nhận diện là cùng 1 agent nếu UUID/file giống nhau |
| Chương trình chạy nhiều giờ | Cẩn thận giải phóng bộ nhớ sau mỗi kết nối, tránh leak trong danh sách agent và buffer đọc input |
| Socket bị đóng bất ngờ | Xử lý `read() <= 0` và đổi trạng thái agent sang OFFLINE |

---

## 12. Bảng đối chiếu Requirement Coverage

| Requirement ID | Mô tả ngắn | Đề cập ở mục | Ghi chú |
|---|---|---|---|
| P4-M1 | Agent đọc /proc và gửi dữ liệu định kỳ | 3, 5 | Thực hiện bằng collector_thread |
| P4-M2 | agent_id cố định, lưu file | 4 | Dùng file `agent.id` |
| P4-M3 | Agent tự hiển thị dashboard riêng | 7 | Dùng thread dashboard |
| P4-M4 | Server dùng epoll | 1, 2 | epoll loop xử lý socket + stdin + timer |
| P4-M5 | 3 loại log tách biệt | 9 | periodic/alert/events |
| P4-M6 | Keepalive 2 lớp | 6 | heartbeat + SO_KEEPALIVE |
| P4-M7 | Phát hiện OFFLINE | 6 | timeout trên last_heartbeat |
| P4-M8 | systemd + không leak | 11 | chạy dài hạn, kiểm tra bằng Valgrind/Heaptrack |
| P4-M9 | Dashboard server động theo số agent | 7 | map agent_id → entry |
| P4-M10 | Lệnh /config | 8 | server gửi config xuống agent |
| P4-M11 | Lệnh /history | 8, 9 | đọc log đã ghi trước đó |

---

## 13. Rủi ro khó nhất trong project

Rủi ro lớn nhất là phối hợp đúng 3 phần cùng lúc: I/O multiplexing với epoll, quản lý state agent động, và đảm bảo không leak bộ nhớ khi chạy lâu. Nếu thiết kế khởi đầu rõ ràng, việc triển khai sẽ dễ kiểm soát hơn nhiều.
