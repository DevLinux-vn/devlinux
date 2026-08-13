# DESIGN.md — Project 2: Smart Weather Alarm Clock

**Học viên:** Lê Phúc Tài | **Ngày nộp:** 13/08/2026

> File này nộp **trước khi mở PR code**. Phần thiết kế nên tập trung vào chỗ dễ sai nhất: đa mục đích của nút nhấn (chuyển màn hình / Smart Config / tắt báo thức), 2 màn hình, và buzzer.

---

## 1. Kiến trúc tổng thể

### Sơ đồ khối hệ thống

```text
+-------------------------------------------------------------------------------------------------------+
|                                           KERNEL SPACE                                                |
|                                                                                                       |
|   +-----------------------+        +-----------------------+        +-----------------------------+   |
|   |   Button Driver       |        |     Buzzer Driver     |        |      I2C Kernel Driver      |   |
|   |  (GPIO Input Ngắt)    |        |    (GPIO Output)      |        |      (SSD1306 OLED)         |   |
|   +-----------+-----------+        +-----------^-----------+        +--------------^--------------+   |
+---------------|--------------------------------|-----------------------------------|------------------+
                | `/dev/btn_driver`              | `/dev/buzzer_driver`              | `/dev/i2c-1`
================|================================|================================---|==================
+---------------v--------------------------------|-----------------------------------|------------------+
|                                           USERSPACE                                |                  |
|  +---------------------------------------------------------------------------------|---------------+  |
|  | MAIN PROCESS (Smart Weather Alarm Clock)                                        |               |  |
|  |                                                                                 |               |  |
|  |    +------------------------------------------------------------------------+   |               |  |
|  |    |                   SHARED SYSTEM STATE (Mutex Protected)                |   |               |  |
|  |    | - current_screen (CLOCK / WEATHER)   - alarm_ringing (true / false)    |   |               |  |
|  |    | - net_mode (STATION / SOFT_AP)       - alarm_config (HH:MM)            |   |               |  |
|  |    | - weather_data (Temp, Condition...)  - wifi_creds (SSID, Password)     |   |               |  |
|  |    +---^----------------^-------------------^----------------^--------------+   |               |  |
|  |        |                |                   |                |                  |               |  |
|  |  +-----+------+   +-----+--------+    +-----+--------+   +---+----------+       |               |  |
|  |  | btn_thread |   | clock_thread |    |weather_thread|   |webserver_    |       |               |  |
|  |  |            |   | (timerfd 1s) |    | (HTTP Client)|   |thread Server |       |               |  |
|  |  +-----+------+   +-----+--------+    +-----+--------+   +---+----------+       |               |  |
|  |        |                |                   |                |                  |               |  |
|  |        |                +-------------------+----------------+                  |               |  |
|  |        |                                    |                                   |               |  |
|  |        v                                    v                                   v               |  |
|  |  +-----------+                      +---------------+                  +-----------------+      |  |
|  |  | Network   |                      | Display       |                  |  buzzer_thread  |      |  |
|  |  | Manager   |                      | Controller    |                  +--------+--------+      |  |
|  |  +-----+-----+                      +-------+-------+                           |               |  |
|  +--------|------------------------------------|-----------------------------------|---------------+  |
|           | (fork/exec)                        +-----------------------------------+                  |
|           |                                                                                           |
|           v                                    v                                   v                  |
|   +-------------------+               +------------------+                +-----------------------+   |
|   |  System Services  |               | Mock Weather     |                | Smartphone / Laptop   |   |
|   | - hostapd         |               | Server (HTTP)    |                | (Web Browser Client)  |   |
|   | - wpa_supplicant  |               +--------^---------+                +-----------^-----------+   |
|   | - dnsmasq         |                        | (HTTP GET /weather)                  | (HTTP GET/POST|
|   +-------------------+                        +--------------------------------------+  Config)      |
|                                                                                                       |
+-------------------------------------------------------------------------------------------------------+
```

### Mô tả luồng hoạt động chính
* **Kernel Layer:** Quản lý giao tiếp phần cứng qua Character Drivers bao gồm nút bấm GPIO ngắt `/dev/btn_driver`, còi `/dev/buzzer_driver` và giao tiếp I2C `/dev/i2c-1` cho màn hình OLED SSD1306.
* **Shared State Center:** Tiến trình chính duy trì cấu trúc dữ liệu chung (Mutex Protected) lưu trữ trạng thái màn hình (`CLOCK`/`WEATHER`), trạng thái báo thức (`alarm_ringing`), cấu hình wifi và giờ báo thức.
* **Luồng xử lý sự kiện & hiển thị:**
  * `btn_thread`: Đọc sự kiện thô từ `/dev/btn_driver`, phân loại theo ngữ cảnh để đổi màn hình, kích hoạt Soft AP hoặc tắt còi.
  * `clock_thread`: Sử dụng `timerfd` định kỳ 1s đồng bộ giờ từ hệ thống (`time()`), kiểm tra báo thức và xuất ra OLED.
  * `weather_thread`: Đóng vai trò HTTP Client truy vấn Mock Weather Server khi chuyển sang màn hình thời tiết hoặc mỗi 5 phút.
  * `webserver_thread`: Cung cấp giao diện web cục bộ cấu hình Wi-Fi (Soft AP) và giờ báo thức.
  * `buzzer_thread`: Điều khiển bật/tắt `/dev/buzzer_driver` dựa trên cờ trạng thái báo thức.
* **Luồng quản lý mạng:** Sử dụng `fork()` + `execvp()` để tương tác với các service hệ thống `hostapd`, `wpa_supplicant`, `dnsmasq` phục vụ cơ chế Soft AP / Station mode.

---

## 2. Danh sách Process/Thread

| Tên | Loại (process/thread) | Vai trò | Tạo lúc nào | Kết thúc lúc nào | Giao tiếp với ai — qua kênh gì |
|---|---|---|---|---|---|
| **main** | process | Khởi tạo Mutex, shared state, mở file driver, tạo worker threads và lắng nghe signal dọn dẹp. | Khởi động chương trình. | Khi nhận signal dừng (SIGINT/SIGTERM) hoặc lỗi fatal. | Quản lý vòng đời tất cả các thread. |
| **btn_thread** | thread | Đọc sự kiện thô từ `/dev/btn_driver`, phân loại ngữ cảnh: tắt còi, chuyển màn hình, Soft AP. | Khi `main` khởi tạo xong driver và mutex. | Chạy suốt vòng đời chương trình (vòng lặp vô hạn). | - **Driver:** đọc blocking `/dev/btn_driver`<br>- **Network:** `fork()`/`execvp()` Soft AP<br>- **Shared State:** Cập nhật state (Mutex) |
| **clock_thread** | thread | Dùng `timerfd` định kỳ 1s thức dậy, đọc `time()`, so sánh giờ báo thức và vẽ giao diện Đồng hồ lên OLED. | Khi `main` khởi tạo lúc startup. | Chạy suốt vòng đời chương trình. | - **I2C Driver:** Ghi ra `/dev/i2c-1`<br>- **Shared State:** Đọc/Ghi state (Mutex) |
| **weather_thread** | thread | HTTP Client. Ở màn hình thời tiết, gửi GET request tới Mock Weather Server (khi chuyển vào hoặc mỗi 5 phút). | Khi `main` khởi tạo lúc startup. | Chạy suốt vòng đời chương trình. | - **Mock Server:** TCP Socket port 80/8080<br>- **Shared State:** Đọc/Ghi state (Mutex) |
| **webserver_thread** | thread | HTTP Server (port 8080). Nhận POST form Wi-Fi (Soft AP) hoặc POST giờ báo thức (Station), lưu vào `alarm.conf`. | Khi `main` khởi tạo lúc startup. | Chạy suốt vòng đời chương trình. | - **Client HTTP:** Socket TCP<br>- **Network:** Gọi script kết nối Station<br>- **Shared State:** Ghi state (Mutex) |
| **buzzer_thread** | thread | Đọc cờ `alarm_ringing`, nếu `true` thì bật còi `/dev/buzzer_driver`, nếu `false` thì tắt. | Khi `main` khởi tạo lúc startup. | Chạy suốt vòng đời chương trình. | - **Buzzer Driver:** Ghi tín hiệu ra `/dev/buzzer_driver`<br>- **Shared State:** Đọc `alarm_ringing` (Mutex) |

### Chi tiết các vùng dữ liệu dùng chung (Shared Data Matrix)
* **`current_screen`**: `btn_thread` (Ghi) $\leftrightarrow$ `clock_thread`, `weather_thread` (Đọc)
* **`alarm_ringing`**: `clock_thread`, `btn_thread` (Ghi) $\leftrightarrow$ `buzzer_thread`, `btn_thread` (Đọc)
* **`alarm_config`**: `webserver_thread` (Ghi) $\leftrightarrow$ `clock_thread` (Đọc)
* **`weather_data`**: `weather_thread` (Ghi) $\leftrightarrow$ `clock_thread`/OLED Display (Đọc)
* **`net_mode`**: `btn_thread`, `webserver_thread` (Ghi) $\leftrightarrow$ `clock_thread`, `webserver_thread` (Đọc)

---

## 3. Resource Mapping — GPIO / I2C

### Bảng cấu hình phần cứng

| Thiết bị | GPIO Pin (Physical Pin) | Cách khai báo | Ghi chú |
|---|---|---|---|
| **Nút nhấn (đa chức năng)** | GPIO 17 (Pin 11) | Device Tree Overlay | Cấu hình `active-low`, bật `pull-up` nội bộ, bắt ngắt GPIO cả 2 cạnh (`IRQ_TYPE_EDGE_BOTH`) để tính thời gian nhấn giữ. |
| **Buzzer** | GPIO 27 (Pin 13) | Device Tree Overlay | Cấu hình GPIO Output (`active-high`), xuất tín hiệu High/Low điều khiển còi. |
| **OLED SSD1306 (128x64)** | SDA: GPIO 2 (Pin 3)<br>SCL: GPIO 3 (Pin 5) | Bus `/dev/i2c-1`<br>Địa chỉ I2C: `0x3C` | Kích hoạt `i2c1` trong Device Tree. Điều khiển trực tiếp từ Userspace thông qua Linux I2C Subsystem (`I2C_SLAVE` ioctl). |

---

## 4. Logic phân loại sự kiện nút nhấn (quan trọng nhất của project)

Nút nhấn dùng chung cho 3 mục đích: chuyển màn hình / vào Smart Config / tắt báo thức. Bảng phân loại chi tiết theo ngữ cảnh:

| Ngữ cảnh hiện tại | Hành động nhấn | Kết quả |
|---|---|---|
| Buzzer đang kêu | Nhấn ngắn | Tắt báo thức ngay lập tức |
| Buzzer không kêu, đang xem 1 trong 2 màn hình | Nhấn ngắn | Chuyển màn hình (`CLOCK` $\leftrightarrow$ `WEATHER`) |
| Bất kỳ lúc nào | Nhấn giữ ≥5s | Vào chế độ Smart Config (Soft AP) |

### Vị trí đặt logic xử lý & Luồng thuật toán
* **Quyết định thiết kế:** Logic phân loại sự kiện theo ngữ cảnh được đặt toàn bộ ở **Tầng Userspace (`btn_thread`)**.
* **Lý do kỹ thuật:**
  * **Đúng nguyên tắc tách biệt Kernel/Userspace:** Kernel Driver chỉ nên quản lý cơ chế ngắt phần cứng (*Mechanism*), không nên nắm thông tin trạng thái ứng dụng cao cấp (*Policy* — như còi có đang kêu hay không, ứng dụng đang ở màn hình nào).
  * **Giúp Driver nhẹ và ổn định:** Driver chỉ cần bắt ngắt GPIO và ghi nhận timestamp chính xác (`timestamp_press`, `timestamp_release`).
  * **Dễ bảo trì và unittest:** Thay đổi logic báo thức hay chuyển màn hình ở Userspace không đòi hỏi phải biên dịch lại Kernel Module.

```text
[Đọc struct button_event từ /dev/btn_driver]
                      |
                      v
             Có phải sự kiện RELEASED?
             /                       \
           (No)                     (Yes)
            /                         \
  [Bỏ qua hoặc lưu           Tính duration = (timestamp_release - timestamp_press)
  timestamp_press]                     |
                                       v
                             duration >= 5000 ms?
                             /                  \
                           (Yes)                (No)
                            /                     \
             [EVENT_LONG_PRESS_5S]            [EVENT_SHORT_PRESS]
                      |                                   |
                      v                                   v
             Chuyển sang Soft AP            Khóa Mutex hệ thống (system_state_mutex)
             (Kích hoạt hostapd)                          |
                                                Is alarm_ringing == true?
                                                /                      \
                                              (Yes)                    (No)
                                               /                         \
                                     Tắt còi báo thức              Đổi màn hình
                                   (alarm_ringing = false)     (CLOCK <-> WEATHER)
                                               \                         /
                                                \                       /
                                              Giải phóng Mutex hệ thống 
```

---

## 5. Thiết kế 2 màn hình hiển thị

**Màn hình đồng hồ:**
- **Cơ chế cập nhật giây:** Sử dụng `timerfd_create(CLOCK_MONOTONIC, 0)` kết hợp `timerfd_settime()` với chu kỳ `it_interval = {1, 0}` để bắn tín hiệu đều đặn mỗi 1 giây.
- **Cách tránh trôi giờ theo thời gian:**
  * Tuyệt đối không dùng lệnh `sleep(1)` kết hợp tự cộng dồn biến đếm thủ công (`i++`), vì độ trễ thực thi lệnh vẽ màn hình sẽ tích lũy dần gây lệch giờ.
  * Thay vào đó, mỗi khi `timerfd` đọc được sự kiện tick, chương trình sẽ lấy giờ thật trực tiếp từ hệ thống (`time()` hoặc `clock_gettime()`) và phân tích cấu trúc thời gian (`localtime()`) để vẽ lại chuỗi `HH:MM:SS`. Nhờ đó, dù tiến trình có bị trễ nhịp đôi chút, giá trị hiển thị vẫn luôn khớp tuyệt đối với đồng hồ hệ thống.

**Màn hình thời tiết:**
- **Thời điểm gọi lấy dữ liệu mới:** Ngay khi vừa chuyển vào màn hình thời tiết + tự động refresh mỗi 5 phút nếu người dùng đứng yên ở màn hình này.
- **Cơ chế non-blocking:** Toàn bộ việc gọi API thời tiết được cô lập trong `weather_thread` chạy độc lập hoàn toàn. Hoàn toàn không thực hiện thao tác mạng chặn (blocking network call) bên trong luồng vẽ đồng hồ hay vòng lặp xử lý sự kiện nút bấm.
- **Xử lý khi mock weather server không phản hồi:**
  * Cấu hình thời gian chờ socket (Socket Timeout) bằng tùy chọn `setsockopt` với `SO_RCVTIMEO` và `SO_SNDTIMEO` (ví dụ: timeout sau 5 giây).
  * Nếu quá thời gian mà server không trả về dữ liệu, socket trả về lỗi, luồng sẽ bắt ngoại lệ, giữ nguyên dữ liệu thời tiết cũ (hoặc hiển thị thông báo lỗi mạng ngắn gọn) và quay lại trạng thái chờ mà tuyệt đối không làm crash hay treo đơ giao diện.

---

## 6. Thiết kế Webserver cấu hình báo thức

### A. Kiến trúc Webserver
* **Mô hình xử lý:** Dùng cơ chế **Blocking `accept()` lặp đơn luồng** trên port 8080 để xử lý tuần tự từng HTTP Request.
* **Phân tích Request thô:** Bóc tách dòng lệnh HTTP đầu tiên để lấy `METHOD` (`GET`/`POST`) và `PATH`, parse body dạng `application/x-www-form-urlencoded`.

### B. Danh sách Routes / Endpoints

| Endpoint | Method | Chế độ mạng | Chức năng | Dữ liệu nhận vào (Body) | Phản hồi (Response) |
|---|---|---|---|---|---|
| `/` | `GET` | Soft AP / Station | Trả về giao diện HTML phù hợp: Form nhập Wi-Fi (nếu Soft AP) hoặc Form đặt giờ báo thức (nếu Station). | *(Không)* | `200 OK` + HTML Form embedded trong C. |
| `/wifi-config` | `POST` | Soft AP | Nhận thông tin SSID và Password Wi-Fi nhà để kết nối. | `ssid=MyHomeWiFi&password=12345678` | `200 OK` + Trang báo đang thử kết nối. |
| `/alarm-config` | `POST` | Station | Nhận giờ/phút báo thức mới và ghi đè lịch cũ. | `hour=07&minute=30` | `200 OK` + Trang báo lưu báo thức thành công. |

### C. Định dạng lưu trữ và cơ chế Ghi đè file cấu hình

* **Chỉ hỗ trợ 1 lịch báo thức duy nhất:** Mỗi lần gửi cấu hình mới, chương trình sẽ mở file ở chế độ ghi đè (`"w"` mode trong C) để thay thế hoàn toàn cấu hình cũ.
* **Định dạng file cấu hình (`Key-Value` đơn giản):**
  ```ini
  hour=07
  minute=30
  enabled=1
  ```
* **Cơ chế ghi đè an toàn (Atomic File Write):** Ghi dữ liệu ra file tạm `alarm.conf.tmp` trước, sau đó gọi `rename("alarm.conf.tmp", "alarm.conf")` để đảm bảo file cấu hình không bị hỏng hóc nếu mất điện đột ngột.

---

## 7. Đồng bộ hoá & Race Condition

### A. Cơ chế bảo vệ vùng nhớ dùng chung (Shared Memory)
Hệ thống sử dụng một Mutex duy nhất để bảo vệ toàn bộ cấu trúc dữ liệu `Shared System State`:
```c
pthread_mutex_t system_state_mutex = PTHREAD_MUTEX_INITIALIZER;
```
* **Lý do thiết kế:** Việc dùng một khóa Mutex toàn cục duy nhất đảm bảo tính đơn giản, loại bỏ 100% rủi ro **Deadlock** (xảy ra khi nhiều khóa bị tranh chấp chéo), đồng thời chi phí lock/unlock trên hệ thống POSIX là cực nhỏ, không làm ảnh hưởng tới hiệu năng hệ thống.

### B. Bảng phân tích nguy cơ Race Condition và Giải pháp

| Dữ liệu dùng chung | Các Thread đụng vào | Nguy cơ Race Condition | Cơ chế giải quyết (Mutex Boundary) |
|---|---|---|---|
| **`alarm_ringing`**<br>*(Boolean)* | - **Write:** `clock_thread` (set `true`), `btn_thread` (set `false`).<br>- **Read:** `btn_thread`, `buzzer_thread`. | `clock_thread` vừa bật còi đúng thời điểm `btn_thread` xử lý ngắt nút bấm để tắt còi, dẫn tới sai lệch trạng thái hiển thị và còi kêu không dừng. | Bọc toàn bộ thao tác đọc/ghi `alarm_ringing` trong block `pthread_mutex_lock()` / `pthread_mutex_unlock()`. |
| **`alarm_config`**<br>*(Struct: hour, minute, enabled)* | - **Write:** `webserver_thread`.<br>- **Read:** `clock_thread`. | Struct gồm nhiều byte. Nếu `clock_thread` đọc giữa chừng khi `webserver_thread` mới ghi được `hour` mà chưa ghi `minute`, hệ thống sẽ so sánh giờ báo thức bị rác (corrupt read). | `webserver_thread` chỉ được ghi đè struct sau khi lấy khóa Mutex. `clock_thread` đọc nguyên bản struct ra bộ đệm cục bộ (local buffer) rồi mới nhả Mutex. |
| **`weather_data`**<br>*(Struct: temp, condition[32], last_update)* | - **Write:** `weather_thread`.<br>- **Read:** `clock_thread` (khi vẽ màn hình). | Mảng chuỗi `condition` gồm nhiều byte. Đọc không đồng bộ sẽ gây hiện tượng vỡ chuỗi hiển thị lên OLED. | `weather_thread` ghi dữ liệu xong mới xin khóa Mutex để `memcpy` vào shared state. `clock_thread` lấy khóa Mutex để copy dữ liệu ra local trước khi vẽ. |
| **`current_screen`**<br>*(Enum: CLOCK/WEATHER)* | - **Write:** `btn_thread`.<br>- **Read:** `clock_thread`, `weather_thread`. | Chuyển đổi màn hình liên tục khi nhấn nút nhanh, khiến `weather_thread` kích hoạt lấy dữ liệu sai thời điểm. | Kiểm tra và cập nhật enum màn hình atomic dưới Mutex. |
| **`net_mode`**<br>*(Enum: STATION/SOFT_AP)* | - **Write:** `btn_thread`, `webserver_thread`.<br>- **Read:** `clock_thread`, `webserver_thread`. | Đổi chế độ mạng đồng thời từ cả nút bấm và web interface. | Tất cả chuyển đổi trạng thái mạng phải thông qua Mutex bảo vệ cờ `net_mode`. |

---

## 8. Edge Case / Failure Handling dự kiến

| Tình huống | Cách xử lý dự kiến |
|---|---|
| **Mất kết nối khi đang ở màn hình thời tiết** | Đặt socket timeout (`SO_RCVTIMEO` = 5s). Nếu fetch thất bại, giữ nguyên dữ liệu cũ, hiển thị icon mất mạng và tiếp tục vòng lặp (không block giao diện, không crash). |
| **Đến giờ báo thức đúng lúc đang ở giữa Smart Config** | `clock_thread` vẫn so sánh giờ độc lập. Khi tới giờ, bật `alarm_ringing = true` và kích buzzer kêu bình thường. Người dùng nhấn ngắn nút để tắt còi mà không bị mất trạng thái Soft AP. |
| **Nhấn giữ nút trong lúc buzzer đang kêu** | Kích hoạt sự kiện nhấn giữ ≥5s: Tắt còi báo thức ngay lập tức đồng thời chuyển hệ thống sang chế độ Smart Config (Soft AP). |
| **Mất điện giữa lúc lưu cấu hình báo thức** | Sử dụng cơ chế Atomic Write: Ghi ra file tạm `alarm.conf.tmp` rồi dùng `rename()` để ghi đè `alarm.conf`. Đảm bảo file cấu hình không bị hư hại (corrupt) nếu mất nguồn đột ngột. |
| **Nhập sai mật khẩu Wi-Fi khi cấu hình Smart Config** | Sau khi nhận thông tin Wi-Fi, hệ thống thử kết nối Station mode trong 15-20s. Nếu thất bại (không đạt `COMPLETED`), tự động ngắt Station và quay lại khởi chạy Soft AP để người dùng nhập lại. |

---

## 9. Bảng đối chiếu Requirement Coverage

| Requirement ID | Mô tả ngắn | Đề cập ở mục | Ghi chú |
|---|---|---|---|
| P2-M1 | Driver nút nhấn + cơ chế Smart Config Soft AP | Mục 2, 3, 4, 6 | Driver bắt ngắt GPIO kèm timestamp; Userspace phân loại giữ ≥5s chuyển Soft AP. |
| P2-M2 | Driver buzzer | Mục 2, 3 | Char driver xuất GPIO output tại `/dev/buzzer_driver`. |
| P2-M3 | Đồng bộ giờ qua NTP | Mục 1, 5 | Dùng `systemd-timesyncd` tự đồng bộ giờ khi kết nối Wi-Fi thành công. |
| P2-M4 | Màn hình đồng hồ, giây nhảy đúng | Mục 2, 5 | Dùng `timerfd` bắn mỗi 1s, đọc giờ từ `time()` chống trôi. |
| P2-M5 | Màn hình thời tiết, refresh đúng nhịp | Mục 2, 5, 8 | Fetch dữ liệu khi chuyển màn hình & định kỳ 5 phút; non-blocking socket timeout 5s. |
| P2-M6 | Nhấn nút chuyển màn hình | Mục 2, 4 | Nhấn ngắn (<5s) khi còi không kêu sẽ đổi `CLOCK` <-> `WEATHER`. |
| P2-M7 | Webserver cấu hình báo thức | Mục 2, 6 | Webserver HTTP C thuần, ghi đè 1 lịch duy nhất vào `alarm.conf`. |
| P2-M8 | Buzzer kêu đúng giờ | Mục 2, 4, 7 | `clock_thread` so sánh giờ hệ thống và bật `alarm_ringing` kích còi. |
| P2-M9 | Tắt buzzer bằng nút nhấn | Mục 2, 4, 7 | Ưu tiên cao nhất: nhấn ngắn khi còi kêu sẽ tắt còi ngay lập tức. |

---

## 10. Rủi ro em thấy khó nhất trong project này

1. **Chuyển đổi Soft AP ↔ Station mode không bị treo giao diện mạng:** Việc điều khiển luồng khởi động/dừng các system services (`hostapd`, `dnsmasq`, `wpa_supplicant`) qua `fork()` + `execvp()` đòi hỏi xử lý timeout và fallback quay lại Soft AP chuẩn xác khi nhập sai mật khẩu Wi-Fi, tránh làm kẹt card mạng `wlan0`.
2. **Phân loại chính xác sự kiện nút nhấn theo 3 ngữ cảnh:** Xử lý sự kiện ngắt GPIO thô từ driver dựa trên mốc thời gian timestamp (bấm ngắn <5s vs. bấm giữ ≥5s) và cờ trạng thái `alarm_ringing` để ưu tiên tắt còi báo thức đúng lúc mà không làm lệch luồng chuyển màn hình.
3. **Đảm bảo OLED nhảy giây mượt mà khi các luồng I/O khác đang hoạt động:** Giữ cho `clock_thread` (`timerfd`) cập nhật giao diện `HH:MM:SS` đúng nhịp từng giây mà không bị khựng/lag do `weather_thread` bị timeout mạng hoặc `webserver_thread` đang thực hiện ghi đè file cấu hình.