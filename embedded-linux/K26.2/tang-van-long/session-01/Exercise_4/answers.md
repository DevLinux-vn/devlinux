# Step 1 — create a regular variable
MY_NAME="Quan"
echo $MY_NAME 
-> terminal: "Quan"
- Có hiển thị vì nó là regular variable nó tồn tại trong shell
hiện tại

# Step 2 — open a child shell and check
bash
echo $MY_NAME        # visible? why?
exit
->terminal: "trống"
- Không hiển thị vì child shell không nhận các regular variable, các biến này không tồn tại trong child shell, nó chỉ sử dụng các biến được khai báo trực tiếp trong nó hoặc các biến đã được export được kế thừa từ parent shell

# Step 3 — export the variable
export MY_NAME="Quan"
bash
echo $MY_NAME        # visible? why?
exit
->tẻminal: "Quan"
- Có hiển thị vì biến MY_NAME đã được export, child shell kế thừa nó từ parent shell nên nó có thể sử dụng được

# Step 4 — modify variable inside child shell
bash
MY_NAME="Alice"
echo $MY_NAME        # what do you see inside child shell?
exit
echo $MY_NAME        # back in parent shell — what do you see? why?
->terminal: "ALice" in chill shell
->terminal: "Quan" in parent shell
- Trong child shell có hiển thị "Alice" nhưng khi exit ra parent shell thì hiển thị là "Quan" vì MY_NAME="ALICE" chỉ được sử dụng trong child shell đến khi thoát ra ngoài thì nó không được parent shell sử dụng khi này parent shell chỉ thấy có biến MY_NAME="Quan"

-Child shell không ảnh hưởng đến parent shell vì child shell và parent shell như 2 chương trình chạy đọc lập. Hai chương trình độc lập thì sẽ không dùng chung biến dùng chung vùng nhớ của nhau, nếu có export thì cũng giống như child shell coppy biến đó và giá trị của biến về dùng, nên mọi thay đổi với biến trong child shell không làm giá trị của biến trong parent shell thay đồi.Child shell là một process riêng biệt và có vùng địa chỉ (address space) riêng. Nó không trực tiếp chia sẻ vùng nhớ với parent shell. Khi child shell được tạo, nó nhận được một bản sao các biến từ parent shell, vì vậy nếu child thay đổi biến thì sẽ không ảnh hưởng đến biến trong parent shell.