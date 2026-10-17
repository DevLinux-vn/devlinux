#!/bin/bash
#Task1: Print value
echo "Task 1:" 
echo "PATH: $PATH"
echo "HOME: $HOME"
echo "USER: $USER"
echo "SHELL: $SHELL"
echo ""
#Task2: Count directories in PATH
echo "Task 2:"
echo "Number of directories in Path: " 
echo $PATH | tr ':' '\n' | wc -l
echo ""
#Task3: list and count environment variables
echo "Task 3:"
echo "All environment variables:"
printenv
echo ""
echo "Total number of environment variables:"
printenv | wc -l

