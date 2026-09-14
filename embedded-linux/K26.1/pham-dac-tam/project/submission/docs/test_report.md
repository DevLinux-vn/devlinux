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
| TC-P4-06 | Pass | TCP keepalive được bật bằng `SO_KEEPALIVE`, `TCP_KEEPIDLE`, `TCP_KEEPINTVL`, `TCP_KEEPCNT`. |
| TC-P4-07 | Pass | Khi agent bị kill hoặc mất kết nối, server chuyển trạng thái về OFFLINE sau timeout. |
| TC-P4-08 | Pass | Valgrind Memcheck chạy 30 phút cho server/agent; ERROR SUMMARY: 0 errors, không có definitely/indirectly/possibly lost. Server còn 1024 bytes still reachable, không phát hiện memory leak.. |
| TC-P4-09 | Pass | Dashboard server cập nhật động khi agent mới kết nối và khi mất kết nối. |
| TC-P4-10 | Pass | Lệnh `/config` được hỗ trợ và gửi config xuống agent tương ứng. |
| TC-P4-11 | Pass | Lệnh `/history` được cấp phát để truy vấn dữ liệu lịch sử qua log. |
| TC-P4-12 | Pass | `/config fake-id interval=5` trả về lỗi xác định agent_id không tồn tại. |

> Mỗi case Pass đều có căn cứ thực tế từ terminal chạy trên máy. Output gốc thể hiện server nhận được dữ liệu và agent hiển thị trạng thái CONNECTED.

## Output thực tế đã quan sát

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


## Tổng kết tự đánh giá
Số case Pass: 12 / Tổng số case: 12

Project đã hoàn thành và kiểm thử đầy đủ các yêu cầu chính của Project 4. Kết quả kiểm thử thực tế cho thấy server và agent build thành công, giao tiếp ổn định qua TCP localhost:9000, agent thu thập và gửi dữ liệu CPU/RAM/DISK thực tế từ hệ thống, dashboard cập nhật động trạng thái và thông tin của agent.