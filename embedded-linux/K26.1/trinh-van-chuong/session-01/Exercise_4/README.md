# Báo cáo Bài tập 3 & Bài tập 4

Bài tập 3 — Đọc và kiểm tra các Biến môi trường
- Giá trị các biến hệ thống đã kiểm tra thành công: `$PATH`, `$HOME`, `$USER`, `$SHELL`.

 Bài tập 4 — Phạm vi hoạt động của Biến (Variable Scope)

 1. Hiện tượng quan sát được qua các bước:
- Bước 1: `echo $MY_NAME` -> Có nhìn thấy giá trị `Quan` vì biến được tạo cục bộ ngay tại shell hiện tại.
- Bước 2: Sau khi gõ `bash` chui vào shell con, `echo $MY_NAME` -> Không nhìn thấy (trả về rỗng).
- Bước 3: Sau khi `export MY_NAME="Quan"` ở shell cha và vào lại shell con -> Có nhìn thấy giá trị `Quan`.
- Bước 4: Sửa thành `MY_NAME="Alice"` ở shell con thì shell con nhận giá trị mới, nhưng khi `exit` về shell cha thì giá trị tại shell cha vẫn giữ nguyên là `Quan`.

