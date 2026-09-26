## Step 1 - Create a regular variable

Commands:
MY_NAME="Quan"
echo $MY_NAME

Output:Quan

Explanation:
Biến `MY_NAME` là một biến thông thường (regular variable) được tạo trong shell hiện tại. Vì vậy shell hiện tại có thể truy cập và hiển thị giá trị của biến.

---

## Step 2 - Open a child shell and check

Commands:
bash
echo $MY_NAME
exit

Output:
(không hiển thị giá trị)

Explanation:
Biến `MY_NAME` không hiển thị trong child shell vì nó chỉ là biến thông thường. Các biến thông thường không được truyền tự động sang child shell.

---

## Step 3 - Export the variable

Commands:
export MY_NAME="Quan"

bash
echo $MY_NAME
exit

Output: Quan

Explanation:

Sau khi sử dụng lệnh `export`, `MY_NAME` trở thành biến môi trường (environment variable). Các biến môi trường sẽ được kế thừa bởi child shell, vì vậy child shell có thể truy cập và hiển thị giá trị của biến.

---

## Step 4 - Modify variable inside child shell

Commands:
bash

MY_NAME="Alice"
echo $MY_NAME

exit

echo $MY_NAME

Output inside child shell:
Alice
Output after returning to parent shell:
Quan

Explanation:

Trong child shell, biến `MY_NAME` được thay đổi thành `Alice` nên lệnh `echo $MY_NAME` hiển thị `Alice`.

Sau khi thoát khỏi child shell, shell cha (parent shell) vẫn hiển thị `Quan` vì child shell chỉ làm việc với một bản sao của biến được kế thừa từ parent shell. Thay đổi trong child shell không ảnh hưởng đến giá trị của biến trong parent shell.

---

# Summary

## 1. Difference between a regular variable and an exported variable

### Regular Variable
- Chỉ tồn tại trong shell hiện tại.
- Không được truyền cho child shell hoặc các tiến trình con.
- Chỉ shell hiện tại mới truy cập được.

### Exported Variable
- Là biến môi trường (environment variable).
- Được truyền cho child shell và các tiến trình con.
- Các shell con có thể truy cập được.

## 2. Why changes made inside a child shell do not affect the parent shell

Parent shell và child shell là hai tiến trình riêng biệt.
Khi child shell được tạo, nó nhận một bản sao các biến môi trường từ parent shell. Nếu child shell thay đổi giá trị của một biến thì chỉ bản sao trong child shell bị thay đổi.