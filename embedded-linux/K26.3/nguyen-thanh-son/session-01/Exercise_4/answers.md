# Step 1 — create a regular variable
MY_NAME="Quan"
echo $MY_NAME        # visible?

**Output:**

```
Quan
```

**Explain:** `MY_NAME="Quan"` tạo một *shell variable* (biến nội bộ) trong tiến trình
shell hiện tại. Biến này nằm trong bảng biến riêng của shell đó, nên `echo` trong cùng
shell đọc được bình thường.

---

# Step 2 — open a child shell and check
bash
echo $MY_NAME        # visible? why?
exit

**Output:**

```    
       <-- Empty
```

**Explain:** Gõ `bash` là mở một **shell con** — một tiến trình mới, hoàn toàn tách biệt.
Shell con chỉ nhận được những biến đã `export`. `MY_NAME` chưa export nên nó không được
truyền xuống → shell con không biết biến này → `echo` in ra dòng trống.

---

# Step 3 — export the variable
export MY_NAME="Quan"
bash
echo $MY_NAME        # visible? why?
exit

**Output:**

```
Quan
```

**Giải thích:** `export` **đánh dấu** biến để nó được đưa vào *environment* của shell.
Từ lúc này, mọi tiến trình con sinh ra sau đó đều nhận được một **bản sao** của
environment, trong đó có `MY_NAME=Quan`. Do đó child shell đọc được giá trị `Quan`.

---

# Step 4 — modify variable inside child shell
bash
MY_NAME="Alice"
echo $MY_NAME        # what do you see inside child shell?
exit
echo $MY_NAME        # back in parent shell — what do you see? why?

**Output:**

```
Alice     <-- trong child shell
Quan      <-- sau khi exit, quay lại parent shell
```

**Explain:**
- Trong child shell: biến được gán lại thành `Alice` nên child in ra `Alice`.
- Sau `exit`: tiến trình child kết thúc, toàn bộ vùng nhớ + bảng biến + environment của
  nó bị hủy. Parent shell vẫn giữ nguyên bản gốc `Quan` của mình.