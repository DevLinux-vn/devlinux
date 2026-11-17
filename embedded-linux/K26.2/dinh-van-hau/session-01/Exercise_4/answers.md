# Step 1 — create a regular variable
MY_NAME="Quan"
echo $MY_NAME        # visible?

=> Thấy kết quả in ra chữ Quan. Bởi vì giá trị của biến được khai báo trong chính shell đó.


------------------------------------------------


# Step 2 — open a child shell and check
bash
echo $MY_NAME        # visible? why?
exit

=> Không thấy chữ Quan. Vì biến MY_NAME được khai báo ở shell cha, shell con không thể truy cập được giá trị của MY_SHELL 

-------------------------------------------------

# Step 3 — export the variable
export MY_NAME="Quan"
bash
echo $MY_NAME        # visible? why?
exit

=> Thấy chữ Quan. Vì biến MY_NAME được export thành biến môi trường, nên cả shell con và shell cha đều có thể truy cập được nó


----------------------------------------------

# Step 4 — modify variable inside child shell
bash
MY_NAME="Alice"
echo $MY_NAME        # what do you see inside child shell?

=> Thấy được chữ Alice. Vì việc khai báo giá trị của biến và in giá trị của biến nằm trong 1 shell con

exit
echo $MY_NAME        # back in parent shell — what do you see? why?

=> Thấy được chữ Quan. Chỉ có thể khai báo trong môi trường shel đó thì mới có thể thấy được giá trị của biến. Ví dụ shell con chỉ có thể nhìn được giá trị của biến nếu nó được khai báo trong chính shell con hoặc giá trị của biến đó được khai báo thành biến môi trường thông qua lệnh export, nó sẽ không được thấy giá trị nếu như nó chỉ được khai báo trong shell cha và ngược lại

