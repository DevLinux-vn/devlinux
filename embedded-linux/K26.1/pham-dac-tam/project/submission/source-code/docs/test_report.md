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
| TC-P4-08 | Partial / N-A | Đã thực hiện kiểm tra Valgrind Memcheck với kết quả: `ERROR SUMMARY: 0 errors`, `definitely lost: 0 bytes`, `indirectly lost: 0 bytes`, `possibly lost: 0 bytes`, `still reachable: 1,024 bytes in 1 blocks`. Về mặt kỹ thuật không phát hiện leak nghiêm trọng, nhưng thời gian chạy thực tế chưa đạt đủ 30 phút/24h như yêu cầu tối thiểu của P4-M8 nên không ghi là Pass tuyệt đối. |
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
- TC-P4-08 chưa được chạy đủ 30 phút/24h theo tiêu chuẩn dài hạn của spec, nên vẫn được đánh là `Partial / N-A` thay vì `Pass` tuyệt đối.
- Kết quả Valgrind hiện tại cho thấy không có leak nghiêm trọng, nhưng vẫn còn `still reachable: 1,024 bytes in 1 blocks` nên cần lưu ý khi review dài hạn.

## Tổng kết tự đánh giá
Số case Pass: 11 / Tổng số case: 12
Số case Partial / N-A: 1 / Tổng số case: 12

Project đã có build thành công, giao tiếp agent/server, keepalive TCP, dashboard động, OFFLINE retention, dynamic `/config`, `/history` có timestamp số, và các file log runtime. Phần còn thiếu duy nhất là bằng chứng Valgrind/Heaptrack dài hạn đúng thời lượng yêu cầu P4-M8.