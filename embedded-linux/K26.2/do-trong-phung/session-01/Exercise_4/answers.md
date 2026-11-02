### Bước 1
### Lệnh
MY_NAME="Quan"
echo $MY_NAME

### Kết quả
Quan

### Giải thích
`MY_NAME="Quan"` tạo một biến shell thông thường (regular shell variable) trong shell hiện tại.
Biến này có thể được sử dụng trong shell hiện tại, vì vậy `echo $MY_NAME` in ra `Quan`
Tuy nhiên, một regular variable không tự động được truyền sang các process con.


## Bước 2
### Lệnh
bash
echo $MY_NAME
exit

### Kết quả
Không có output được in ra.

### Giải thích

`MY_NAME` là một regular shell variable và chưa được export.
Khi parent shell tạo một child shell bằng lệnh `bash` thì các regular shell variable không được tự động truyền sang child shell.
Do đó, trong child shell `echo $MY_NAME` không in ra gì vì child shell không có biến `MY_NAME`.

Sau khi chạy `exit` child shell kết thúc và chúng ta quay lại parent shell.

## Bước 3

### Lệnh
export MY_NAME="Quan"
bash
echo $MY_NAME
exit

### Kết quả
Quan

### Giải thích

Lệnh `export MY_NAME="Quan"` đánh dấu `MY_NAME` là một exported variable.
Khi một biến được export, nó trở thành một phần của môi trường và được truyền cho các process con.
Vì vậy, khi parent shell tạo child shell, child shell sẽ nhận được `MY_NAME=Quan`
Do đó `echo $MY_NAME` trong child shell sẽ in ra `Quan`

Sự khác nhau chính là:
**Regular variable** chỉ có trong shell hiện tại.
**Exported variable** được truyền sang các process con.

## Bước 4
### Các lệnh
bash
MY_NAME="Alice"
echo $MY_NAME
exit
echo $MY_NAME

### Kết quả
Alice
Quan

### Giải thích
Khi child shell được tạo, nó nhận được exported variable `MY_NAME=Quan`
Bên trong child shell, chúng ta thay đổi giá trị `MY_NAME="Alice"`
Vì vậy `echo $MY_NAME` trong child shell sẽ in `Alice`
Tuy nhiên, thay đổi này chỉ ảnh hưởng đến biến trong child shell.
Child shell có một bản sao riêng của environment từ parent shell. Nó không sử dụng chung vùng biến với parent shell.
Do đó, khi child shell thoát bằng `exit` parent shell vẫn giữ `MY_NAME=Quan`
Vì vậy lệnh `echo $MY_NAME` sau khi quay lại parent shell sẽ in `Quan`

### Tổng kết

Một regular shell variable chỉ tồn tại và được sử dụng trong shell hiện tại. Nó không tự động được kế thừa bởi các process con.
Một exported variable được đưa vào environment của shell và được truyền sang các process con. Vì vậy child shell có thể truy cập được biến này.
Khi child shell thay đổi một biến, nó chỉ thay đổi bản sao của biến trong child shell. Thay đổi đó không được truyền ngược về parent shell.