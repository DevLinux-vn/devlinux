#!/bin/bash

echo "=== 1. Basic environment variables ==="
echo "PATH  = $PATH"
echo "HOME  = $HOME"
echo "USER  = $USER"
echo "SHELL = $SHELL"
echo

echo "=== 2. Number of directories in PATH ==="
echo "$PATH" | tr ':' '\n' | wc -l
echo

echo "=== 3. All environment variables ==="
printenv
echo
echo "Total environment variables:" $(printenv | wc -l)