# TEST REPORT — <Tên project> — Học viên: ___________

> File này nộp cùng PR code, đặt tại `docs/test_report.md`. Điền đúng theo danh sách Test Case của project tương ứng (`TESTCASES_Px_student.md`). Học viên **tự chạy** toàn bộ test case trên máy/thiết bị của mình — reviewer sẽ không tự chạy lại, chỉ đối chiếu báo cáo này với code.
>
> **Về bằng chứng:** Claude (reviewer tự động) chỉ đọc được **text** — không xem ảnh, không xem video. Toàn bộ ảnh/video học viên chụp/quay (nếu có) chỉ dành cho **mentor con người** xem đối chứng khi cần, đặt trong `docs/demo/`.
>
> **Bắt buộc: mỗi case phải có LOG chương trình dán nguyên văn**, không chỉ mô tả bằng lời. Log ở đây là **output thật** khi chạy (terminal output, `journalctl`, log file chương trình tự ghi ra, hoặc debug print nếu chương trình có in ra hành động đang thực hiện). Claude sẽ đọc log này và **đối chiếu trực tiếp với source code** — kiểm tra log có đúng định dạng/nội dung mà các dòng `printf`/`log`/`ESP_LOGI`... trong code thực sự sẽ tạo ra hay không. Log giả (không khớp với bất kỳ câu lệnh in log nào trong code) sẽ bị phát hiện và tính là bằng chứng không hợp lệ.
> - Nếu chương trình hiện tại chưa in log rõ ràng cho hành động cần kiểm tra, học viên nên **chủ động thêm log** vào code (ví dụ: in ra khi relay đổi trạng thái, khi wifi kết nối thành công, khi nhận lệnh `/config`...) — vừa giúp debug, vừa là bằng chứng test hợp lệ.
> - Mô tả bằng lời (cột "Mô tả kết quả quan sát được") vẫn cần điền, dùng để giải thích ý nghĩa của log, nhưng **không thay thế được log**.
>
> **Giới hạn độ dài log (cơ chế tự động, không cần học viên tự đếm dòng):**
> - Hệ thống (`review_pr.py`) chỉ lấy **10 dòng đầu tiên** của mỗi khối log trước khi đưa cho Claude — dán bao nhiêu dòng cũng được, nhưng **chỉ 10 dòng đầu tiên mới được dùng để chấm điểm**, phần còn lại bị bỏ qua hoàn toàn.
> - Vì vậy: **bắt buộc đặt đoạn log quan trọng nhất lên đầu mỗi khối** (đúng thời điểm xảy ra sự kiện cần kiểm chứng). Không dán nguyên cả phiên chạy dài rồi để đoạn cần thiết nằm ở giữa/cuối — Claude sẽ không thấy được phần đó.
> - Áp dụng luôn cho TC-P4-08 (Valgrind/Heaptrack): đặt dòng tóm tắt quan trọng nhất (ví dụ dòng `LEAK SUMMARY` hoặc kết luận `definitely lost` / `no leaks are possible`) trong 10 dòng đầu, thay vì để nó chìm giữa hàng trăm dòng liệt kê allocation.

**Ngày chạy test:** ___________
**Môi trường chạy test:** (VD: Raspberry Pi Zero W, Yocto build ngày ___, hoặc: Ubuntu 22.04 VM)

---

## Bảng kết quả

| Test ID | Kết quả (Pass/Fail/N-A) | Mô tả kết quả quan sát được |
|---|---|---|
| TC-XX-01 | | (mô tả ngắn gọn ý nghĩa của log tương ứng ở mục "Log chi tiết" bên dưới) |
| TC-XX-02 | | |
| ... | | |

> Log của từng case không ghi trong bảng này — ghi ở mục **"Log chi tiết"** bên dưới, dưới đúng heading trùng Test ID (ví dụ log của `TC-XX-01` nằm dưới heading `### TC-XX-01`). Không cần điền gì thêm trong bảng để "trỏ" tới log — Test ID giống nhau ở 2 chỗ là đủ để đối chiếu.
> Với mỗi case **Fail**: bắt buộc ghi rõ nguyên nhân đã xác định được (hoặc "chưa xác định được nguyên nhân" nếu thật sự chưa tìm ra) — không để trống.
> Với mỗi case **N/A**: ghi rõ lý do không áp dụng được (case này chỉ hợp lệ nếu Nice-to-have liên quan không được làm).
> Mô tả không được ghi chung chung kiểu "hoạt động đúng" — phải cụ thể (SSID cụ thể, nội dung text trên OLED, giờ:phút:giây cụ thể...), và **luôn có log nguyên văn kèm theo** ở mục dưới.

## Log chi tiết (bắt buộc — dán nguyên văn, không paraphrase; chỉ 10 dòng đầu mỗi khối được dùng để chấm — đặt đoạn quan trọng lên đầu)

### TC-XX-01
```
(dán log thật ở đây — copy trực tiếp từ terminal/journalctl/log file, giữ nguyên định dạng gốc; đặt dòng quan trọng nhất trong 10 dòng đầu)
```

### TC-XX-02
```
(...)
```

*(lặp lại cho từng Test ID)*

## Vấn đề đã biết nhưng chưa fix (nếu có)

Liệt kê trung thực các lỗi/hạn chế học viên đã biết nhưng chưa kịp sửa trước khi nộp — việc khai báo trung thực ở đây được đánh giá tích cực hơn là để reviewer tự phát hiện ra mà không được báo trước.

```
(1-3 gạch đầu dòng, hoặc ghi "không có")
```

## Tổng kết tự đánh giá

Số case Pass: ___ / Tổng số case: ___

```
(1-2 câu học viên tự nhận xét mức độ hoàn thành so với yêu cầu)
```
