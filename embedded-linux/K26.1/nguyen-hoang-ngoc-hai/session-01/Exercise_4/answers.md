Step 1
MY_NAME="Quan"
echo $MY_NAME

Kết quả:

Quan

Giải thích:

MY_NAME là một biến thông thường (regular variable) được tạo trong shell hiện tại. Vì vậy khi sử dụng echo $MY_NAME, shell có thể đọc được giá trị và hiển thị ra màn hình.

Step 2
bash
echo $MY_NAME

Kết quả:




(rỗng)

Giải thích:

Khi mở một child shell bằng lệnh bash, shell con sẽ không tự động kế thừa các biến thông thường từ shell cha. Do biến MY_NAME chưa được export nên child shell không nhìn thấy biến này.

Step 3
export MY_NAME="Quan"
bash
echo $MY_NAME

Kết quả:

Quan

Giải thích:

Lệnh export biến MY_NAME thành biến môi trường (environment variable). Các biến môi trường sẽ được kế thừa bởi các tiến trình con và child shell. Vì vậy khi mở child shell, biến MY_NAME vẫn tồn tại và có thể được hiển thị.

Step 4
bash
MY_NAME="Alice"
echo $MY_NAME

Kết quả trong child shell:

Alice

Thoát khỏi child shell:

exit
echo $MY_NAME

Kết quả trong parent shell:

Quan

Giải thích:

Khi gán:

MY_NAME="Alice"

ta chỉ thay đổi giá trị của biến trong child shell hiện tại. Child shell hoạt động như một tiến trình riêng và chỉ nhận bản sao môi trường từ parent shell. Do đó các thay đổi trong child shell không ảnh hưởng đến parent shell. Sau khi thoát child shell, biến trong parent shell vẫn giữ nguyên giá trị ban đầu là Quan.