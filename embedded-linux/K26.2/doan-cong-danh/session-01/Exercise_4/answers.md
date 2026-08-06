# Step 1 — create a regular variable
MY_NAME="Quan"
echo $MY_NAME        # visible?
##  Quan
## 
# Step 2 — open a child shell and check
bash
echo $MY_NAME        # visible? why?
exit
## empty. Vì khi chạy bash tạo ra một child shell, MY_NAME chưa được export nên chỉ tồn tại trong parent shell
# Step 3 — export the variable
export MY_NAME="Quan"
bash
echo $MY_NAME        # visible? why?
exit
## Quan. Vì đã export nên MY_NAME thành environment variable. Các biến môi trường từ parent shell sẽ được truyền sang chil sheld, nên child shell có thể truy cập
# Step 4 — modify variable inside child shell
bash
MY_NAME="Alice"
echo $MY_NAME        # what do you see inside child shell? 
exit
echo $MY_NAME        # back in parent shell — what do you see? why?
## child shell: Alice
## parent shell: QUAN. Vì thay đổi MY_NAME thành Alice trong child shell thì chỉ bản sao của MY_NAME trong child shell thay đổi, parent shell không đổi nên khi exit thì các thay đổi trong child shell sẽ bị thay đổi

# Regular variable: Chỉ tôn tại trong shell hiện tại, không được truyền cho child shell
# Exported variable; Tồn tại trong shell hiện tại, truyền được cho child shell
# Vì child shell là một tiến trình độc lập. Khi đươc tạp ra nó chỉ nhận bản sao của các biến môi trường của parent shell. Mọi thay đổi trong child shell chỉ ảnh hưởng đến bản sao đó.