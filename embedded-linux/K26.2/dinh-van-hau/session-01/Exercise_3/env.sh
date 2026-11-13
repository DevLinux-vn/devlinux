#!/bin/bash

echo "$PATH"
echo "$HOME"
echo "$USER"
echo "$SHELL"

echo "$PATH" | tr ':' '\n' | wc -l
# Xuất ra số lượng thư mục trong PATH -> traslate (tr) dấu : thành xuống dòng -> word count theo lines (wc -l)

printenv | wc -l