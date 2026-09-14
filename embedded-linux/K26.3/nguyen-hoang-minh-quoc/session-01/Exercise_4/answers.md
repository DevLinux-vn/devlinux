# Exercise 4 — Biến trong shell cha và shell con

## Bước 1
Kết quả: Quan.
MY_NAME là biến thông thường, được tạo trong shell hiện tại.

## Bước 2
Kết quả: dòng trống.
Khi chạy bash, một shell con được tạo. MY_NAME chưa được export
nên không được truyền vào môi trường của shell con.
Lệnh exit đưa tôi trở về shell cha.

## Bước 3
Kết quả trong shell con: Quan.
Lệnh export đưa MY_NAME vào môi trường để các tiến trình con
được tạo sau đó có thể kế thừa.

## Bước 4
Trong shell con, kết quả là Alice.
Sau khi exit về shell cha, kết quả vẫn là Quan.

Shell con nhận một bản sao môi trường của shell cha khi được tạo.
Việc sửa biến trong shell con chỉ tác động đến bản sao đó,
không làm thay đổi giá trị trong shell cha.