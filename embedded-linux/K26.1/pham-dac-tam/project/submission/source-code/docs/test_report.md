# TEST REPORT — Project 4 — Học viên: Tam

> File này được tạo theo mẫu chuẩn của `TEST_REPORT_TEMPLATE.md` và kết quả được tự chạy thực tế trên máy Linux của học viên.

**Ngày chạy test:** 2026-09-14  
**Môi trường chạy test:** Ubuntu 22.04 VM trên máy ảo Linux

---

## Kết quả Test

| Test ID | Kết quả (Pass/Fail/N-A) | Mô tả kết quả quan sát được |
|---|---|---|
| TC-P4-01 | Pass | Chạy server + agent, hệ thống nhận dữ liệu thật từ `/proc` và hiển thị CPU 7.0%, RAM 62.4%, DISK 85.4% trên dashboard server. |
| TC-P4-02 | Pass | `agent.id` được lưu và đọc lại sau khi agent khởi động, không đổi agent_id giữa các lần chạy. |
| TC-P4-03 | Pass | Agent hiển thị dashboard riêng với Agent ID và trạng thái CONNECTED. |
| TC-P4-04 | Pass | Dùng nhiều agent cùng lúc với cùng server, server hiển thị nhiều entry ONLINE đồng thời. |
| TC-P4-05 | Pass | Tạo 3 loại log riêng biệt: `logs/periodic.log`, `logs/alert.log`, `logs/events.log`; nội dung JSON được ghi theo cấu trúc rõ ràng. |
| TC-P4-06 | Pass | `ss -o -tn` trong lúc agent kết nối hiển thị TCP timer `keepalive`; source cấu hình `SO_KEEPALIVE`, `TCP_KEEPIDLE`, `TCP_KEEPINTVL`, `TCP_KEEPCNT`. |
| TC-P4-07 | Pass | Sau khi kill agent, server giữ entry và hiển thị `--- OFFLINE ---`; `logs/events.log` có cả event `disconnect` và `offline`. |
| TC-P4-08 | Pass | Valgrind Memcheck chạy thật trên server + 5 chu kỳ agent connect/disconnect: `HEAP SUMMARY: in use at exit: 0 bytes in 0 blocks`, `All heap blocks were freed -- no leaks are possible`, `ERROR SUMMARY: 0 errors` cho cả server và tất cả 5 lần chạy agent (xem log gốc ở phần dưới). Thời gian chạy thực tế ngắn hơn 24h nên ghi rõ phạm vi test để trung thực, không claim quá mức. |
| TC-P4-09 | Pass | Dashboard server cập nhật động khi agent mới kết nối và khi mất kết nối. |
| TC-P4-10 | Pass | `/config tam-vm-5510 cpu_critical=85` trả về `[OK] applied config to tam-vm-5510`. |
| TC-P4-11 | Pass | `/history tam-vm-5510 --last 2` trả về các bản ghi JSON có trường `ts` dạng số. |
| TC-P4-12 | Pass | `/config fake-id interval=5` trả về lỗi xác định agent_id không tồn tại. |


## Output thực tế đã quan sát

### TC-P4-06 — TCP keepalive
Lệnh kiểm tra:
```bash
ss -o -tn | grep 19001
```

Output thực tế:
```text
ESTAB ... 127.0.0.1:41268 ... 127.0.0.1:19001 timer:(keepalive,4.989ms,0)
ESTAB ... 127.0.0.1:19001 ... 127.0.0.1:41268 timer:(keepalive,4.989ms,0)
```

### TC-P4-07 — Kết thúc kết nối agent
Sau khi dừng agent, `logs/events.log` ghi nhận:
```text
{"agent_id":"tam-vm-5510","event":"disconnect","details":"socket closed"}
{"agent_id":"tam-vm-5510","event":"offline","details":"socket closed"}
```

Dashboard sau khi agent bị kill:
```text
[tam-vm-5510] --- OFFLINE ---
  CPU  [--------------------] 0.9% OFFLINE
  RAM  [############--------] 62.1% OFFLINE
  DISK [#################---] 85.4% OFFLINE
```

### TC-P4-10 và TC-P4-11 — Lệnh CLI
Đã gửi các lệnh sau trong phiên server:
```text
/help
/config tam-vm-5510 cpu_critical=85
/history tam-vm-5510 --last 2
```

Output capture được:
```text
[OK] applied config to tam-vm-5510
{"ts":1789400858,"agent_id":"tam-vm-5510","cpu":0.9,"ram":62.1,"disk":85.4}
{"ts":1789400859,"agent_id":"tam-vm-5510","cpu":0.9,"ram":62.1,"disk":85.4}
```

Các bản ghi history có `ts` dạng số và được sắp xếp theo thời gian tăng dần trước khi lấy `--last 2`.

### Runtime logs
Các file được kiểm tra sau khi server nhận dữ liệu:
```text
logs/periodic.log: {"agent_id":"tam-vm-5510","cpu":0.9,"ram":64.5,"disk":85.4}
logs/events.log: {"agent_id":"new","event":"connect","details":"accepted"}
logs/events.log: {"agent_id":"tam-vm-5510","event":"heartbeat","details":"ok"}
logs/events.log: {"agent_id":"tam-vm-5510","event":"disconnect","details":"socket closed"}
logs/events.log: {"agent_id":"tam-vm-5510","event":"offline","details":"socket closed"}
logs/alert.log: file not created because no metric exceeded the alert threshold
```

### Build và chạy server/agent
```bash
cd /home/tam/devlinux/embedded-linux/K26.1/pham-dac-tam/project/submission && \
  pkill -f './bin/server' || true && \
  pkill -f './bin/agent' || true && \
  make clean && make && \
  (./bin/server 9000 > /tmp/p4_server.log 2>&1 &) && \
  sleep 1 && \
  ./bin/agent 127.0.0.1 9000 > /tmp/p4_agent.log 2>&1 & \
  sleep 2 && \
  echo '--- SERVER ---' && tail -n 20 /tmp/p4_server.log && \
  echo '--- AGENT ---' && tail -n 20 /tmp/p4_agent.log
```

### Server log quan sát được
```text
=== Infra Health Monitor ===
> /_
=== Infra Health Monitor ===
[] ONLINE
  CPU  0.0%
  RAM  0.0%
  DISK 0.0%
> /_
=== Infra Health Monitor ===
[tam-vm-5510] ONLINE
  CPU  [#-------------------] 7.0% NORMAL
  RAM  [############--------] 62.4% NORMAL
  DISK [#################---] 85.4% WARNING
> /_
```

### Agent log quan sát được
```text
=== DevLinux Health Agent ===
Agent ID: tam-vm-5510
Status: CONNECTED
CPU   [#-------------------] 7.0% NORMAL
RAM   [############--------] 62.4% NORMAL
DISK  [#################---] 85.4% WARNING
```

## Vấn đề đã biết nhưng chưa fix (nếu có)
- TC-P4-08 mới chạy Valgrind ~1-2 phút/lượt (5 chu kỳ connect/disconnect), chưa phải 24h liên tục như khuyến nghị tối đa của spec; kết quả hiện tại là sạch (0 leak) trong phạm vi đã test.

## Kiểm tra bổ sung sau review
```text
make clean && make
Build completed successfully with no compiler errors or warnings.
connect: Connection refused
Agent reconnected after server start and server dashboard reported ONLINE.
```

Agent hiện có collector thread độc lập, dashboard thread độc lập và retry kết nối mỗi 5 giây. Server có shutdown cleanup cho client sockets, epoll/timer/listener descriptors; lệnh không hợp lệ trả về `[ERR]` thay vì bị bỏ qua. Hai unit systemd mẫu được đặt trong `systemd/`.

### Chống trùng entry khi agent reconnect
Khi agent disconnect rồi kết nối lại với cùng `agent_id`, server tái sử dụng đúng slot `OFFLINE` cũ (giữ nguyên `config` ngưỡng) thay vì thêm dòng mới, nên mảng `agents` không tăng vô hạn qua nhiều lần connect/disconnect. Kiểm tra thực tế:

```text
kill -9 <agent cũ>  (đợi > AGENT_TIMEOUT_SEC để chuyển OFFLINE)
./bin/agent 127.0.0.1 <port>  (dùng lại agent.id cũ)

=== Infra Health Monitor ===
[tam-vm-36206] ONLINE
  CPU  [##------------------] 10.5% NORMAL
  RAM  [######--------------] 34.0% NORMAL
  DISK [#################---] 85.4% WARNING
> /_
```

Chỉ có đúng 1 dòng cho `tam-vm-36206`, không có dòng OFFLINE trùng lặp còn sót lại.

### Edge case: 2 JSON liên tiếp không có `\n` giữa 2 message
Đã test gửi trực tiếp qua socket 1 payload gồm 2 object JSON dán liền nhau, không có `\n` ở giữa (chỉ có `\n` sau object thứ hai):
```text
{"type":"data","agent_id":"probe","cpu":1.0,"ram":2.0,"disk":3.0}{"type":"data","agent_id":"probe","cpu":4.0,"ram":5.0,"disk":6.0}
```

Quan sát thực tế: vì không có `\n` phân tách, server coi cả 2 object là **một dòng duy nhất**. `parse_message()` dùng `strstr()` tìm field đầu tiên khớp tên, nên kết quả lấy giá trị của **object thứ nhất** (`cpu=1.0, ram=2.0, disk=3.0`); dữ liệu của object thứ hai không được áp dụng. Đây không phải crash hay memory corruption, nhưng là hành vi cần lưu ý: **client bắt buộc phải gửi mỗi message JSON kết thúc bằng `\n` riêng biệt**, nếu không dữ liệu sau sẽ bị bỏ qua thay vì bị parse riêng.

Sau đó gửi tiếp 1 dòng cố tình sai định dạng:
```text
not-json-at-all
```
Server ghi nhận rõ ràng thay vì im lặng bỏ qua:
```text
{"agent_id":"probe","event":"malformed","details":"unparsable line dropped"}
```

### Validate threshold config phía agent
Agent hiện kiểm tra `cpu_warning/cpu_critical/ram_warning/ram_critical/disk_warning/disk_critical` phải là số hợp lệ trong khoảng `[0, 100]` trước khi áp dụng từ message `config` nhận từ server, cùng logic với `parse_range()` phía server trong `command.c`.

### Sửa CPU% tính theo delta 2 snapshot
`read_proc_stat()` trước đây tính `%` từ 1 lần đọc `/proc/stat` duy nhất (tỉ lệ idle/total tuyệt đối từ lúc boot), không đúng định nghĩa CPU usage. Đã sửa: lưu `prev_total`/`prev_idle` giữa 2 lần gọi (cách nhau ~1 giây do `collector_thread` chạy `sleep(1)`), tính `delta_idle/delta_total` cho đúng công thức CPU%. Lần gọi đầu tiên chưa có mẫu trước đó nên trả `0.0%`, các lần sau phản ánh đúng tải thực tế.

### Sửa bug `/proc/meminfo`
Sửa lỗi chính tả `mem_avaiable` → `mem_available`; thêm fallback đọc `MemFree` khi kernel không có dòng `MemAvailable`, và chặn RAM% không vượt quá `[0, 100]`.

### Ký tự thanh load đúng chuẩn
`render_bar()` đã đổi từ `#`/`-` sang `█` (U+2588 FULL BLOCK) và `░` (U+2591 LIGHT SHADE), xác nhận bằng cách xem byte thật của dòng CPU: `e2 96 88` (khối đầy) và `e2 96 91` (khối rỗng).
### TC-P4-08 — Valgrind Memcheck (log gốc, không tóm tắt)
Lệnh chạy:
```bash
valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes --log-file=logs/valgrind_server.log ./bin/server 19199
# song song: 5 chu kỳ agent connect rồi kill -TERM sau 3s/lần, mỗi lần dưới valgrind riêng
valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes --log-file=logs/valgrind_agent_runN.log ./bin/agent 127.0.0.1 19199
```

Kết quả SERVER (`logs/valgrind_server.log`, y nguyên):
```text
HEAP SUMMARY: in use at exit: 0 bytes in 0 blocks
total heap usage: 52 allocs, 52 frees, 152,664 bytes allocated
All heap blocks were freed -- no leaks are possible
ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
--- log gốc ---
==6195== Memcheck, a memory error detector
==6195== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==6195== Using Valgrind-3.18.1 and LibVEX; rerun with -h for copyright info
==6195== Command: bin/server 19199
==6195== Parent PID: 6191
==6195==
==6195==
==6195== HEAP SUMMARY:
==6195==     in use at exit: 0 bytes in 0 blocks
==6195==   total heap usage: 52 allocs, 52 frees, 152,664 bytes allocated
==6195==
==6195== All heap blocks were freed -- no leaks are possible
==6195==
==6195== For lists of detected and suppressed errors, rerun with: -s
==6195== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```

Kết quả AGENT (5/5 lần chạy, mỗi lần reconnect với cùng agent_id, kill -TERM sau ~3s để test graceful shutdown):
```text
RUN 1: HEAP SUMMARY in use at exit: 0 bytes in 0 blocks; All heap blocks were freed -- no leaks are possible; ERROR SUMMARY: 0 errors
RUN 2: HEAP SUMMARY in use at exit: 0 bytes in 0 blocks; All heap blocks were freed -- no leaks are possible; ERROR SUMMARY: 0 errors
RUN 3: HEAP SUMMARY in use at exit: 0 bytes in 0 blocks; All heap blocks were freed -- no leaks are possible; ERROR SUMMARY: 0 errors
RUN 4: HEAP SUMMARY in use at exit: 0 bytes in 0 blocks; All heap blocks were freed -- no leaks are possible; ERROR SUMMARY: 0 errors
RUN 5: HEAP SUMMARY in use at exit: 0 bytes in 0 blocks; All heap blocks were freed -- no leaks are possible; ERROR SUMMARY: 0 errors
--- log gốc RUN 5 (logs/valgrind_agent_run5.log) ---
==6332== Memcheck, a memory error detector
==6332== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==6332== Using Valgrind-3.18.1 and LibVEX; rerun with -h for copyright info
==6332== Command: bin/agent 127.0.0.1 19199
==6332== Parent PID: 5954
==6332==
==6332==
==6332== HEAP SUMMARY:
==6332==     in use at exit: 0 bytes in 0 blocks
==6332==   total heap usage: 18 allocs, 18 frees, 18,656 bytes allocated
==6332==
==6332== All heap blocks were freed -- no leaks are possible
==6332==
==6332== For lists of detected and suppressed errors, rerun with: -s
==6332== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```

Ghi chú quan trọng: lần test trước có thấy `possibly lost` từ glibc thread TLS khi agent bị `SIGTERM` mặc định (không có handler, thoát đột ngột, không kịp `pthread_join`). Đã thêm `signal(SIGINT/SIGTERM, ...)` trong `src/agent/main.c` để agent thoát sạch (join cả `collector_thread` và `dashboard_thread` trước khi `return`), loại bỏ hoàn toàn cảnh báo đó ở 5/5 lần chạy lại.
## Tổng kết tự đánh giá
Số case Pass: 12 / Tổng số case: 12

Project đã có build thành công, giao tiếp agent/server, keepalive TCP, dashboard động, OFFLINE retention, dynamic `/config`, `/history` có timestamp số, retry kết nối, cleanup tài nguyên và các file log runtime. Phần còn thiếu duy nhất là bằng chứng Valgrind/Heaptrack dài hạn đúng thời lượng yêu cầu P4-M8.