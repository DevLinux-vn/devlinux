#!/bin/bash
# Exercise_3 - env.sh

echo "===== 1) Print PATH, HOME, USER, SHELL ====="
echo "PATH  = $PATH"
echo "HOME  = $HOME"
echo "USER  = $USER"
echo "SHELL = $SHELL"

echo
echo "===== 2) Count how many directories are in \$PATH ====="
# $PATH entries are separated by ':' -> replace ':' with newline, then count lines
echo "$PATH" | tr ':' '\n' | wc -l

echo
echo "===== 3) List all environment variables and count them ====="
printenv
echo "----- number of environment variables -----"
printenv | wc -l
