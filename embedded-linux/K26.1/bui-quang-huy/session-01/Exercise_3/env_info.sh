#!/bin/bash

echo "=== Environment Variables ==="
echo "PATH  : $PATH"
echo "HOME  : $HOME"
echo "USER  : $USER"
echo "SHELL : $SHELL"

echo ""
echo "=== PATH Directory Count ==="
dir_count=$(echo "$PATH" | tr ':' '\n' | wc -l)
echo "Number of directories in PATH: $dir_count"

echo ""
echo "=== All Environment Variables ==="
printenv

echo ""
echo "=== Total Environment Variable Count ==="
env_count=$(printenv | wc -l)
echo "Total environment variables: $env_count"
