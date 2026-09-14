# README — Project 4: Infrastructure Health Monitoring

## 1. Tổng quan
Project này xây dựng hệ thống giám sát hạ tầng Linux theo mô hình client-server:
- Agent thu thập CPU, RAM, DISK và gửi định kỳ lên server qua TCP.
- Server lắng nghe kết nối từ nhiều agent, cập nhật trạng thái ONLINE/OFFLINE, lưu log và hiển thị dashboard.

Mục tiêu chính là đáp ứng các yêu cầu từ P4-M1 đến P4-M11 theo đề bài.

## 2. Cấu trúc thư mục
```text
project/submission/
├── DESIGN.md
├── README.md
├── Makefile
├── agent.id
├── bin/
├── docs/
│   └── test_report.md
├── logs/
├── src/
│   ├── agent/
│   │   ├── agent_id.c
│   │   ├── agent_id.h
│   │   ├── dashboard.c
│   │   ├── main.c
│   │   └── metrics.c
│   ├── server/
│   │   ├── command.c
│   │   ├── common.c
│   │   ├── config.c
│   │   ├── dashboard.c
│   │   ├── keepalive.c
│   │   ├── logger.c
│   │   └── main.c
│   └── common.h
└── ...
```

## 3. Build project
```bash
cd /home/tam/devlinux/embedded-linux/K26.1/pham-dac-tam/project/submission
make clean && make
```

## 4. Chạy server
Mở terminal 1:
```bash
cd /home/tam/devlinux/embedded-linux/K26.1/pham-dac-tam/project/submission
./bin/server 9000
```

## 5. Chạy agent
Mở terminal 2:
```bash
cd /home/tam/devlinux/embedded-linux/K26.1/pham-dac-tam/project/submission
./bin/agent 127.0.0.1 9000
```

## 6. Kiểm tra nhanh / Test case cơ bản
```bash
cd /home/tam/devlinux/embedded-linux/K26.1/pham-dac-tam/project/submission
pkill -f './bin/server' || true
pkill -f './bin/agent' || true
make clean && make
./bin/server 9000
```
Terminal khác:
```bash
cd /home/tam/devlinux/embedded-linux/K26.1/pham-dac-tam/project/submission
./bin/agent 127.0.0.1 9000
```

Kiểm tra trạng thái kết nối:
```bash
ss -o -tn | grep 9000
```

## 7. Test Valgrind (Memory check)
Bắt buộc kiểm tra rò rỉ bộ nhớ theo yêu cầu P4-M8. Trước khi chạy, đảm bảo đã cài `valgrind`:
```bash
sudo apt-get update
sudo apt-get install -y valgrind
```

### 7.1. Chạy server dưới Valgrind
```bash
cd /home/tam/devlinux/embedded-linux/K26.1/pham-dac-tam/project/submission
mkdir -p logs
timeout 1800s valgrind \
  --tool=memcheck \
  --leak-check=full \
  --show-leak-kinds=all \
  --track-origins=yes \
  --error-exitcode=1 \
  --log-file=logs/valgrind_server.log \
  ./bin/server 9000
```

### 7.2. Chạy agent dưới Valgrind
Terminal khác:
```bash
cd /home/tam/devlinux/embedded-linux/K26.1/pham-dac-tam/project/submission
timeout 1800s valgrind \
  --tool=memcheck \
  --leak-check=full \
  --show-leak-kinds=all \
  --track-origins=yes \
  --error-exitcode=1 \
  --log-file=logs/valgrind_agent.log \
  ./bin/agent 127.0.0.1 9000
```

### 7.3. Đọc kết quả Valgrind
```bash
grep -E "ERROR SUMMARY|definitely lost|indirectly lost|possibly lost|still reachable" \
  logs/valgrind_server.log logs/valgrind_agent.log
```

Kết quả tốt cần là:
- `ERROR SUMMARY: 0 errors`
- `definitely lost: 0 bytes`
- `indirectly lost: 0 bytes`
- `possibly lost: 0 bytes`

Lưu ý: `still reachable` không phải là lỗi nghiêm trọng nếu bộ nhớ còn được giữ bởi runtime, nhưng nên ghi rõ trong báo cáo test nếu xuất hiện.

## 8. Giao thức Agent ↔ Server
Project dùng JSON-line qua TCP socket.

### Data message (agent gửi lên server)
```json
{"type":"data","agent_id":"tam-vm-5510","cpu":7.0,"ram":62.4,"disk":85.4}
```

### Heartbeat message
```json
{"type":"heartbeat","agent_id":"tam-vm-5510"}
```

### Config message (server gửi xuống agent)
```json
{"type":"config","agent_id":"tam-vm-5510","key":"interval","value":"5"}
```

## 9. Lệnh quản trị trên server
Khi server đang chạy, nhập các lệnh sau vào prompt `/_`:
```text
/help
/config <agent_id> <key>=<value>
/history <agent_id> [--last N]
```

Ví dụ:
```text
/config tam-vm-5510 interval=5
/config tam-vm-5510 cpu_warning=70
/config tam-vm-5510 cpu_critical=85
/config tam-vm-5510 ram_warning=75
/config tam-vm-5510 ram_critical=90
/config tam-vm-5510 disk_warning=80
/config tam-vm-5510 disk_critical=95
/history tam-vm-5510 --last 5
```

Các ngưỡng được lưu riêng trong từng `AgentEntry` và áp dụng cho dashboard server. Khi agent đang kết nối, server gửi cùng thay đổi xuống agent để dashboard agent dùng cùng quy tắc màu:
- `NORMAL`: xanh
- `WARNING`: vàng
- `CRITICAL`: đỏ
- agent mất kết nối: `--- OFFLINE ---` và các thanh màu đỏ

## 10. Log hệ thống
- `logs/periodic.log`: dữ liệu định kỳ
- `logs/alert.log`: cảnh báo vượt ngưỡng
- `logs/events.log`: sự kiện kết nối, timeout, offline

## 11. Ghi chú
- Nếu port 9000 đang bị chiếm, dùng:
```bash
pkill -f './bin/server' || true
pkill -f './bin/agent' || true
```
- Hoặc:
```bash
fuser -k 9000/tcp
```
- Dữ liệu CPU/RAM/DISK được đọc trực tiếp từ `/proc` và hệ thống Linux, không dùng thư viện giám sát riêng.
