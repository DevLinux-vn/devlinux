
### Questions

Answer in `isr_notes.md`, one short paragraph each:

**1. Why must the shared button-state variable be `volatile`? What could happen if the main loop reused a cached value?**

Dùng `volatile` để báo cho trình biên dịch (compiler) biết rằng giá trị của biến có thể thay đổi bất cứ lúc nào bởi phần cứng hoặc ngắt (ISR), ngăn chặn việc compiler tối ưu hóa bằng cách cache biến vào thanh ghi CPU. Nếu không có `volatile`, vòng lặp chính (main loop) có thể tái sử dụng giá trị cũ đã được cache trong thanh ghi CPU, dẫn đến việc không đọc được giá trị mới nhất được cập nhật bởi ISR và làm sai logic của chương trình.

**2. Why should the ISR only save state and timestamps? Why do logging and delays belong in the main loop, and why is `vTaskDelay()` invalid inside the ISR?**

Để giữ cho các lệnh thực thi trong ISR là ngắn nhất, tránh giữ CPU trong interrupt quá lâu khiến cho các task khác/interrupt bị block, logging/delays/`vTaskDelay()` là những hàm khiến interrupt bị giữ lâu.

**3. Compare polling and interrupts: edge-detection latency, time until the display updates, CPU usage while idle, and implementation difficulty. State your timing assumptions; interrupts do not automatically make the display respond faster.**

| Tiêu chí so sánh | Polling (Quét liên tục) | Interrupts (Ngắt phần cứng) |
| :--- | :--- | :--- |
| **Edge-detection latency** *(Độ trễ bắt cạnh)* | **Chậm hơn** <br> Phụ thuộc vào thời gian vòng lặp quét tới lệnh đọc. | **Nhanh hơn (Tức thời)** <br> Phần cứng tự động phát hiện và ép CPU nhảy vào xử lý ngay lập tức. |
| **Time until display updates** *(Thời gian cập nhật LED)* | **Như nhau** <br> Sau khi nhận diện xong, logic cập nhật LED đều do vòng lặp chính xử lý. | **Như nhau** <br> Cùng thực thi một đoạn code cập nhật LED như Polling. |
| **CPU usage while idle** *(Tài nguyên CPU khi rảnh)* | **Cao (100% tài nguyên)** <br> CPU phải chạy vòng lặp liên tục để đọc trạng thái chân. | **Thấp (Tối ưu)** <br> CPU được nghỉ ngơi (Idle), chỉ thức dậy khi có tín hiệu ngắt. |
| **Implementation difficulty** *(Độ khó triển khai)* | **Thấp (Đơn giản)** <br> Chỉ cần đọc giá trị và dùng lệnh trễ (`vTaskDelay`). | **Cao hơn** <br> Phải cấu hình ISR, xử lý biến `volatile`, chống dội non-blocking và FSM. |

> **Kết luận:** Interrupt không làm cho màn hình hiển thị nhanh hơn, nhưng nó giúp tiết kiệm tài nguyên cho CPU hơn rất nhiều so với việc dùng Polling phải quét trạng thái liên tục.