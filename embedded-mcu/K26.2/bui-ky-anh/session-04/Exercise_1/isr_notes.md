# ISR notes - Session 04

## 1. Vì sao biến dùng chung giữa ISR và task cần `volatile`?

Nếu một biến thông thường chỉ bị sửa ở ISR, trình biên dịch không nhất thiết coi ISR là một luồng thực thi bất đồng bộ có thể thay đổi biến đó. Trong `while (!flag)`, nó có thể đọc `flag` một lần, giữ giá trị trong thanh ghi và biến vòng lặp thành chờ vô hạn; các lần `if (flag)` cũng có thể dùng giá trị đã lưu thay vì đọc lại bộ nhớ. Việc ISR gán biến không tự buộc các lần đọc ở task phải được thực hiện lại. `volatile` buộc trình biên dịch thực hiện các truy cập quan sát được, nhưng **không thay thế tính nguyên tử, khóa hay đồng bộ giữa các core**.

Trong chương trình này, `timestamp_us` và `is_press` được sao chép qua `xQueueSendFromISR()` / `xQueueReceive()`, vì vậy không cần khai báo chúng là `volatile`. Queue đảm nhiệm việc trao đổi và đồng bộ. Chỉ `dropped_edges` là bộ đếm 32-bit `volatile`, có một ISR ghi và task chỉ đọc, không reset hay cùng thực hiện read-modify-write. Các truy cập thanh ghi hiển thị đều thông qua con trỏ `volatile uint32_t *`.

`xQueueSendFromISR()` không chờ queue có chỗ trống. Tham số `pxHigherPriorityTaskWoken` được khởi tạo `pdFALSE`; nếu gửi sự kiện làm một task ưu tiên cao hơn sẵn sàng chạy, ISR gọi `portYIELD_FROM_ISR()` để yêu cầu chuyển ngữ cảnh khi thoát ngắt.

## 2. Vì sao không gọi `ESP_LOGI()` hoặc `vTaskDelay()` trong ISR?

`ESP_LOGI()` có thể đi qua định dạng chuỗi, khóa đồng bộ và hàm xuất console; đường gọi này không bảo đảm an toàn trong ISR và có thể đụng khóa mà task đang bị ngắt giữ. Nếu ISR cần chờ khóa đó thì task không thể tiếp tục để nhả khóa, gây deadlock hoặc vi phạm quy tắc RTOS. Khi cache flash tạm bị tắt, gọi hàm hoặc đọc dữ liệu ở flash còn có thể làm lỗi truy cập. Chỉ gắn `IRAM_ATTR` lên hàm ngoài không tự đưa mọi hàm con và dữ liệu của nó vào RAM.

`vTaskDelay()` thao tác với trạng thái và danh sách chờ của **task đang chạy**, yêu cầu ngữ cảnh task; ISR không có task riêng để đưa vào trạng thái blocked. Gọi nó từ ISR có thể gây assert, hỏng trạng thái scheduler hoặc treo. ISR dài cũng làm tăng độ trễ của các ngắt khác và có thể kích hoạt watchdog.

ISR của bài chỉ đọc mức nút, lấy thời gian, gửi queue và yêu cầu yield nếu cần. Debounce, nhận diện gesture, ghi display và log đều nằm trong task. Nút vẫn được cấu hình bằng Driver API; riêng ISR đọc một lần `GPIO_IN_REG` qua con trỏ volatile. Trong ESP-IDF 5.2, `CONFIG_GPIO_CTRL_FUNC_IN_IRAM` chuyển một số hàm điều khiển vào RAM nhưng không chuyển `gpio_get_level()`. Vì vậy ISR không gọi hàm này. `button_isr`, `esp_timer_get_time()` và hàm queue FromISR đã được kiểm tra vị trí IRAM trong firmware sau khi build.

## 3. So sánh polling và interrupt

**Độ đáp ứng:** nếu polling đọc mỗi `P` ms, cạnh nút phải chờ thêm từ gần 0 tới gần `P` ms mới được nhận biết. Bản interrupt bắt cạnh tại ISR nên bỏ được khoảng chờ lấy mẫu này; task vẫn chịu độ trễ ngắt, lập lịch và độ phân giải timeout. Cả hai bản vẫn cần debounce 25 ms, đợi tối đa 350 ms để phân biệt click đơn với click đôi, và ngưỡng giữ 800 ms. Vì thế interrupt không làm click đơn xuất hiện ngay lập tức. Ví dụ nếu polling có `P = 5 ms`, phần chờ phát hiện cạnh giảm tối đa khoảng 5 ms; số này là phân tích, **không phải kết quả đo trên board**. Không có mã polling Session 03 đã hoàn thành trong nguồn được cung cấp, nên chưa thể nêu một số đo chênh lệch chính xác cho bản đó.

**CPU lúc không bấm:** task của bản interrupt bị chặn vô thời hạn trong `xQueueReceive()` khi không có gesture hay deadline đang chờ. Nó không định kỳ thức dậy để đọc pin. Bản polling có delay vẫn phải thức dậy mỗi `P` ms để kiểm tra; polling bận còn chiếm nhiều CPU hơn. Interrupt giảm phần việc kiểm tra nút lúc rảnh, nhưng không có nghĩa mọi phần khác của hệ thống cũng ngừng chạy hay tự vào deep sleep.

**Độ khó:** về cách thiết kế, polling dễ theo dõi hơn vì kiểm tra mức nút và xử lý thời gian nằm trong cùng một vòng lặp. Bản interrupt cần bảo đảm ISR ngắn, API FromISR đúng, queue không mất cạnh và timeout vẫn chạy khi giữ nút không phát sinh cạnh mới. Logic debounce, click đơn đang chờ, click đôi, giữ lâu và wrap 0-9 vẫn được giữ về ý nghĩa; phần đọc pin định kỳ được thay bằng cạnh có timestamp và deadline. Chương trình này chờ cạnh ổn định 25 ms bằng thời gian của sự kiện, không đọc lại chân nút trong vòng lặp. Sau long press, cạnh release chỉ kết thúc giữ, không tạo thêm click.

**Giới hạn xác minh:** đã kiểm tra build và logic trên máy; chưa đo latency/CPU hay chạy thử display, nút bấm trên board thật. Các nhận xét trên là so sánh thiết kế, không phải báo cáo đo thực nghiệm.
