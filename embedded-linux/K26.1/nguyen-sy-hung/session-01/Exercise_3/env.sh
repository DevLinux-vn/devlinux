#!/bin/bash

echo "===== Environment Variables ====="

echo "PATH : $PATH"
echo "HOME : $HOME"
echo "USER : $USER"
echo "SHELL: $SHELL"

echo
echo "===== Number of directories in PATH ====="

path_count=$(echo "$PATH" | tr ':' '\n' | wc -l)
echo "Directories in PATH: $path_count"

echo
echo "===== All Environment Variables ====="

printenv

echo
echo "===== Total Environment Variables ====="

env_count=$(printenv | wc -l)
echo "Total environment variables: $env_count"