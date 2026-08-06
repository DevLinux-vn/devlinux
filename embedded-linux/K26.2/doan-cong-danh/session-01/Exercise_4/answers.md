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
echo $MY_NAME        # what do you see inside child shell
exit
echo $MY_NAME        # back in parent shell — what do you see? why?
## child shell: Alice
## parent shell: Quan. Vì thay đổi MY_NAME thành Alice trong child shell thì chỉ bản sao của MY_NAME trong child shell thay đổi, parent shell không đổi nên khi exit thì các thay đổi trong child shell sẽ bị thay đổi

# Regular variable: Chỉ tôn tại trong shell hiện tại, không được truyền cho child shell
# Exported variable; Tồn tại trong shell hiện tại, truyền được cho child shell
# Khi user chạy bash, bash hiện tại (parent bash) sẽ tạo ra một tiến trình mới gọi là child shell (độc lâoj với parent shell). Khi child shell được tạo ra nó sẽ nhân một bản sao của các vvariables11 environment từ parent shell. Các variables chưa được export sẽ không được sao chép. Khi child shell được tạo ra, child shell và parent shell hoạt động như hai tiến trình độc lập (mỗi tiến trình có vùng nhớ riêng). Vì là bản sao nên khi child shell thay đổi giá trị của các varibles, parent shell sẽ không bị ảnh hưởng