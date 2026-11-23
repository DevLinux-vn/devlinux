# ISR Notes — Session 04

**Exercise 1: Interrupt-Driven 7-Segment Counter**

---

## Câu 1: Tại sao biến trạng thái nút dùng chung phải được khai báo `volatile`?

Biến trạng thái dùng chung cần được khai báo **`volatile`** vì giá trị của nó có thể bị thay đổi bất đồng bộ bởi **ISR (Interrupt Service Routine)**.

Nếu không sử dụng `volatile`, trình biên dịch có thể tối ưu hóa bằng cách tái sử dụng giá trị đã đọc trước đó thay vì đọc lại từ bộ nhớ. Điều này khiến vòng lặp chính không phát hiện được sự thay đổi trạng thái nút nhấn do ISR cập nhật.

Tuy nhiên, `volatile` **không đảm bảo tính nguyên tử (atomicity) và đồng bộ dữ liệu**. Vì vậy, cần sử dụng **critical section** để đọc nhất quán trạng thái nút và các dấu thời gian (*timestamps*).

---

## Câu 2: Tại sao ISR chỉ nên lưu trạng thái và dấu thời gian?

Hàm xử lý ngắt (**ISR**) chỉ nên lưu trạng thái nút và dấu thời gian vì ISR cần thực thi nhanh để tránh làm tăng độ trễ xử lý các tác vụ và ngắt khác.

Các công việc như **ghi nhật ký (logging), tạo độ trễ (delay) và xử lý FSM** nên được thực hiện trong vòng lặp chính vì chúng có thể tốn thời gian hoặc gây chặn.

Hàm **`vTaskDelay()` không hợp lệ trong ISR** vì đây là API của FreeRTOS dành cho task, có chức năng đưa task vào trạng thái *Blocked* và nhường CPU cho task khác. Trong khi đó, ISR chạy trong **ngữ cảnh ngắt (interrupt context)**, không phải một task, nên không thể sử dụng cơ chế trì hoãn này.

---

## Câu 3: So sánh phương pháp Polling và Interrupt

Giả sử trong **Session 03**, chương trình kiểm tra nút nhấn bằng **Polling mỗi 5 ms**, còn **Session 04** đọc dữ liệu do ISR lưu lại **mỗi 10 ms**.

Trong điều kiện lập lịch lý tưởng, Polling phát hiện cạnh tín hiệu sau khoảng **0–5 ms**, trong khi Interrupt có thể ghi nhận cạnh nhanh hơn nhiều, thường ở mức microsecond trong điều kiện thuận lợi.

Tuy nhiên, LED trong phương pháp Interrupt vẫn có thể cập nhật chậm hơn vì FSM chỉ được xử lý mỗi 10 ms. Ngoài ra, chương trình còn phải xét đến các ngưỡng thời gian:

- **Debounce:** 25 ms.
- **Double-click window:** 350 ms.
- **Long-press threshold:** 800 ms.
- **Long-press repeat:** 500 ms.

Polling phải đọc GPIO định kỳ, còn Interrupt không cần đọc GPIO khi nút không thay đổi trạng thái, mặc dù vòng lặp chính vẫn được đánh thức mỗi 10 ms.

Việc triển khai Interrupt phức tạp hơn vì cần cấu hình ISR, khai báo biến `volatile`, đồng bộ hóa và bảo vệ dữ liệu dùng chung.

**Kết luận:** Interrupt giúp cải thiện tốc độ phát hiện cạnh tín hiệu nhưng **không tự động làm giảm độ trễ cập nhật LED**.