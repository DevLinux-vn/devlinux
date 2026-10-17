Step 1 — Create a regular variable

MY_NAME="Quan"
echo $MY_NAME
=>> Result:Quan

Why: 
    - MY_NAME là một biến shell thông thường. Nó tồn tại trong shell hiện tại nên có thể được truy cập bằng $MY_NAME.

Step 2 — Open a child shell and check

bash
echo $MY_NAME
exit
=>> Result:<empty>

Why: 
    - Khi chạy bash, một child shell mới được tạo. 
    - Biến MY_NAME ở parent shell là biến shell thông thường, không phải environment variable, nên nó không được truyền sang child shell. 
    - Nói cách khác, child shell có một environment riêng và chỉ nhận các biến được export từ parent shell.

Step 3 — Export the variable

export MY_NAME="Quan"
bash
echo $MY_NAME
exit
=>> Result:Quan

Why: 
    - export đánh dấu MY_NAME là một exported variable (environment variable). 
    - Khi parent shell tạo child shell, giá trị của các biến được export sẽ được truyền vào environment của child shell. 
    - Vì vậy child shell có thể truy cập $MY_NAME và nhận được giá trị Quan.

Step 4 — Modify the variable inside child shell

bash
MY_NAME="Alice"
echo $MY_NAME
exit
echo $MY_NAME
=>> Result inside child shell:Alice
=>> Result after returning to parent shell:Quan

Why: 
    - Trong child shell, câu lệnh: MY_NAME="Alice" chỉ thay đổi bản sao của biến MY_NAME trong child shell.
    - Child shell là một process riêng, có state riêng. 
    - Việc thay đổi biến trong child shell không thể thay đổi ngược lại state của parent shell. 
    - Do đó: Child shell thấy MY_NAME="Alice".
    - Sau exit, child shell kết thúc và thay đổi đó biến mất. Parent shell vẫn giữ MY_NAME="Quan".