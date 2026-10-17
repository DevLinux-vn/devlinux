Giải thích các bước.

Bước 1
	Output: Quan 
	Quan là regular shell variable nên nó sẽ tồn tại và 
	xuất được qua lệnh echo trong shell hiện tại.
Bước 2
	Output: no result
	Vì Quan chỉ là biến regular nên khi ta tạo child shell 
	thì nó sẽ không hiểu được biến Quan
Bước 3
	Output: Quan 
	Sau khi sử dụng lệnh export biến Quan sẽ trở thành 
	environment variable.
Bước 4
	Output: Alice-child shell; Quan-parent shell 
	Tại vì khi ta thực hiện thay đổi ở child shell thì nó 	chỉ đang thay đổi bản copy từ parent mà nó tạo ra chứ 	không thay đổi biến môi trường này tại parent shell

