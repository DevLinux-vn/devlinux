# isr_notes.md — Session 04, Exercise_1

## 1. Vì sao `btn_pressed` phải khai báo `volatile`?

`btn_pressed` được **ISR** ghi và được **main loop** đọc — hai luồng thực thi
hoàn toàn khác nhau (ISR chạy bất đồng bộ, không theo trình tự các lệnh trong
`app_main`). Compiler không biết điều này.

Nếu không có `volatile`, khi tối ưu hoá (`-O2`), compiler thấy trong main loop
biến `btn_pressed` không hề bị thay đổi bởi bất kỳ lệnh nào *trong vòng lặp
đó* (theo góc nhìn của nó), nên nó có thể:

- Đọc giá trị của `btn_pressed` **một lần**, cache vào registor, rồi dùng lại
  giá trị cũ mãi mãi (loại bỏ hẳn việc đọc lại từ RAM ở mỗi vòng lặp).
- Hoặc tệ hơn, loại bỏ hoàn toàn nhánh `if (btn_pressed)` nếu nó "chứng minh"
  được rằng theo logic tuần tự, biến này không đổi.

Hậu quả: main loop **không bao giờ phát hiện** được cờ mà ISR đã set — nút
bấm coi như vô tác dụng, dù phần cứng và ISR vẫn hoạt động đúng. `volatile`
buộc compiler phải đọc/ghi trực tiếp vào bộ nhớ mỗi lần truy cập, không cache.

## 2. Vì sao gọi `ESP_LOGI()` hoặc `vTaskDelay()` trực tiếp trong `button_isr()` là nguy hiểm?

ISR chạy trong **ngữ cảnh ngắt** (interrupt context), không phải một FreeRTOS
task bình thường:

- `vTaskDelay()` gọi vào scheduler và có thể **block/yield** — nhưng ISR
  không được phép yield cho task khác theo cách thông thường; gọi hàm này
  trong ISR dẫn đến crash hoặc hành vi không xác định (assert của FreeRTOS
  sẽ bắt lỗi này nếu dùng đúng API `FromISR`).
- `ESP_LOGI()` bên trong dùng `printf`/UART driver, có thể cấp phát bộ nhớ
  (`malloc`), dùng mutex, hoặc block để chờ UART trống — tất cả đều là các
  thao tác **không an toàn trong ISR** vì ISR không được phép chờ
  (block) hoặc giữ mutex.
- ISR cần hoàn thành **cực nhanh** (vài micro-giây) để không làm trễ các
  ngắt khác hoặc toàn hệ thống. Ghi log qua UART có thể tốn hàng chục đến
  hàng trăm micro-giây, làm ISR "treo" hệ thống trong lúc đó.

Đó là lý do quy tắc: ISR chỉ set cờ (`btn_pressed = true`), toàn bộ logic
thực (đổi `led_state`, gọi log, điều khiển LED) đẩy ra `app_main()` — nơi
chạy trong ngữ cảnh task bình thường, được phép block/log/gọi driver API.

## 3. Debounce cho nút bấm

Một nút bấm cơ khí khi nhấn/thả có thể sinh ra nhiều xung nhiễu (rung tiếp
điểm) trong khoảng vài ms, khiến ISR bị gọi nhiều lần cho một lần nhấn thật.

**Chiến lược debounce đơn giản: lọc theo thời gian (time-based debounce)
bằng timestamp, xử lý ở main loop — không xử lý trong ISR.**

Ý tưởng:

- Lưu thời điểm (tick) của lần xử lý nhấn hợp lệ gần nhất.
- Khi cờ `btn_pressed` được set và main loop xử lý, so sánh thời điểm hiện
  tại với thời điểm lần trước; chỉ coi là một lần nhấn "thật" nếu khoảng
  cách đủ lớn (ví dụ > 50 ms). Nếu không, bỏ qua (coi như nhiễu do rung).

```c
static volatile bool btn_pressed = false;
static TickType_t last_press_tick = 0;
#define DEBOUNCE_TICKS pdMS_TO_TICKS(50)

/* trong main loop */
if (btn_pressed) {
    btn_pressed = false;

    TickType_t now = xTaskGetTickCount();
    if ((now - last_press_tick) > DEBOUNCE_TICKS) {
        last_press_tick = now;

        led_state = !led_state;
        /* ... đổi LED như cũ ... */
    }
    /* else: nhiễu do rung tiếp điểm, bỏ qua */
}
```

Ưu điểm của cách này: ISR vẫn cực đơn giản (chỉ set cờ), toàn bộ logic thời
gian/so sánh nằm ở main loop — đúng nguyên tắc "ISR làm tối thiểu". Nhược
điểm: nếu người dùng bấm rất nhanh (nhanh hơn 50 ms) thì lần bấm đó bị coi
là dội và bị bỏ qua — cần chọn `DEBOUNCE_TICKS` phù hợp với đặc tính nút
thực tế (thường 20–50 ms là hợp lý cho nút bấm cơ khí thông thường).

Một cách khác (không dùng trong bài này nhưng có thể nêu thêm): debounce
bằng phần cứng (thêm tụ lọc RC ở chân nút) hoặc dùng bộ đếm xác nhận trạng
thái ổn định trong N lần đọc liên tiếp (polling debounce) — nhưng vì đây là
thiết kế interrupt-driven, cách lọc theo timestamp ở trên là phù hợp và đơn
giản nhất để thêm vào code hiện tại.
