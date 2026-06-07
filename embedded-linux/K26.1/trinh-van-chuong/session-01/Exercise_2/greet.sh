#!/bin/bash

# Kiểm tra nếu tổng số tham số ($#) ít hơn 2
if [ $# -lt 2 ]; then
    echo "Cú pháp chuẩn: ./greet.sh <tên> <tuổi>"
    exit 1
fi

# Nếu đủ tham số thì chạy đoạn dưới này
echo "Hello, my name is $1 and I am $2 years old."
echo "Total arguments received: $#"