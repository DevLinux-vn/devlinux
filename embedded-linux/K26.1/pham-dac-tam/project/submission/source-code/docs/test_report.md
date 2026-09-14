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
| TC-P4-07 | Partial / N-A | Khi agent dừng, server ghi nhận event `disconnect/socket closed`. Chưa có output chứng minh agent vẫn giữ trên dashboard với trạng thái `OFFLINE` sau heartbeat timeout. |
| TC-P4-08 | Partial / N-A | Đã thực hiện kiểm tra Valgrind Memcheck với kết quả: `ERROR SUMMARY: 0 errors`, `definitely lost: 0 bytes`, `indirectly lost: 0 bytes`, `possibly lost: 0 bytes`, `still reachable: 1,024 bytes in 1 blocks`. Về mặt kỹ thuật không phát hiện leak nghiêm trọng, nhưng thời gian chạy thực tế chưa đạt đủ 30 phút/24h như yêu cầu tối thiểu của P4-M8 nên không ghi là Pass tuyệt đối. |
| TC-P4-09 | Pass | Dashboard server cập nhật động khi agent mới kết nối và khi mất kết nối. |
| TC-P4-10 | Partial / N-A | Source có xử lý `/config`, nhưng lần chạy evidence hiện tại chưa capture được output `[OK] applied config ...` và message config ở phía agent. |
| TC-P4-11 | Partial / N-A | Source có xử lý `/history`, nhưng lần chạy evidence hiện tại chưa capture được output của lệnh trên terminal server. |
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
```

Output này chứng minh server nhận biết socket bị đóng. Chưa ghi nhận được dòng `event":"offline"` hoặc dashboard giữ lại agent ở trạng thái `OFFLINE`, vì vậy TC-P4-07 không được coi là Pass tuyệt đối.

### TC-P4-10 và TC-P4-11 — Lệnh CLI
Đã gửi các lệnh sau trong phiên server:
```text
/help
/config tam-vm-5510 interval=5
/history tam-vm-5510 --last 5
```

Output capture được:
```text
/help
/config <agent_id> <key>=<value>
/history <agent_id> [--last N]
```

Chưa capture được output xác nhận `[OK] applied config ...` hoặc các dòng history trả về ngay sau hai lệnh, nên hai test này được ghi là `Partial / N-A` trong bảng kết quả.

### Runtime logs
Các file được kiểm tra sau khi server nhận dữ liệu:
```text
logs/periodic.log: {"agent_id":"tam-vm-5510","cpu":0.9,"ram":64.5,"disk":85.4}
logs/events.log: {"agent_id":"new","event":"connect","details":"accepted"}
logs/events.log: {"agent_id":"tam-vm-5510","event":"heartbeat","details":"ok"}
logs/events.log: {"agent_id":"tam-vm-5510","event":"disconnect","details":"socket closed"}
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
  CPU  7.0%
  RAM  62.4%
  DISK 85.4%
> /_
```

### Agent log quan sát được
```text
=== DevLinux Health Agent ===
Agent ID: tam-vm-5510
Status: CONNECTED
CPU   [#-------------------] 7.0%
RAM   [############--------] 62.4%
DISK  [#################---] 85.4%
```

## Vấn đề đã biết nhưng chưa fix (nếu có)
- TC-P4-08 chưa được chạy đủ 30 phút/24h theo tiêu chuẩn dài hạn của spec, nên vẫn được đánh là `Partial / N-A` thay vì `Pass` tuyệt đối.
- Kết quả Valgrind hiện tại cho thấy không có leak nghiêm trọng, nhưng vẫn còn `still reachable: 1,024 bytes in 1 blocks` nên cần lưu ý khi review dài hạn.

## Tổng kết tự đánh giá
Số case Pass: 8 / Tổng số case: 12
Số case Partial / N-A: 4 / Tổng số case: 12

Project đã có build thành công, giao tiếp agent/server, keepalive TCP, dashboard và các file log runtime. Các phần còn thiếu bằng chứng trực tiếp là trạng thái OFFLINE giữ trên dashboard, kết quả Valgrind dài hạn, output xác nhận `/config`, và output `/history`; các phần này được ghi rõ là partial thay vì pass tuyệt đối.