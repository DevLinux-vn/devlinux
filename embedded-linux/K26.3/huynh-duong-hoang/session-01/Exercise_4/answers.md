# Step 1 — create a regular variable
MY_NAME="Quan"
echo $MY_NAME        # visible?
-> Có thấy in ra. Vì biến được thực thi trong Shell hiện tại


# Step 2 — open a child shell and check
bash
echo $MY_NAME        # visible? why?
exit
-> Không in ra. Vì biến hiện tại là biến local trong Shell hiện tại và bash tạo ra 1 Shell con độc lập với Shell chính nên không kế thừa bất kì biến nào của Shell chính

# Step 3 — export the variable
export MY_NAME="Quan"
bash
echo $MY_NAME        # visible? why?
exit
-> Có in ra. Biến được đưa thành biến môi trường nên khi bash tạo Shell con sẽ được kế thừa biến của Shell chính (chưa rõ lắm về chỗ này)

# Step 4 — modify variable inside child shell
bash
MY_NAME="Alice"
echo $MY_NAME        # what do you see inside child shell?
-> Có in ra giá trị Alice. vì biến chỉ được khai báo trong Shell con

exit
echo $MY_NAME        # back in parent shell — what do you see? why?
-> Có in ra giá trị Quan. Vì biến Alice được tạo trong Shell con mà Shell con độc lập với Shell chính nên khi chỉnh sửa biến MY_NAME trong Shell con sẽ không ảnh hưởng đến Shell chính
