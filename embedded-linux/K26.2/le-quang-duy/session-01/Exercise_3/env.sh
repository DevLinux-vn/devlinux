#!/bin/bash

echo "=== 1. ENVIRONMENT VARIABLE ==="
echo "PATH: $PATH"
echo "HOME: $HOME"
echo "USER: $USER"
echo "SHELL: $SHELL"
echo ""

echo "=== 2. NUMBER OF FOLDER IN PATH ==="
echo "$PATH" | tr ':' '\n' | wc -l
echo ""

echo "=== 3. LIST ENVIRONMENT VARIABLES ==="
printenv

echo ""
echo "-> TOTAL ENVIRONMENT VARIABLES:"
printenv | wc -l