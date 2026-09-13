#!/bin/bash

# Task 1: Print the value of $PATH, $HOME, $USER, $SHELL
echo "PATH: $PATH"
echo "HOME: $HOME"
echo "USER: $USER"
echo "SHELL: $SHELL"

# Task 2: Count how many directories are in $PATH
echo -n "Number of directories in $PATH: "
echo "$PATH" | tr ':' '\n' | wc -l

# Task 3: Use printenv to list all environment variables and count how many there are
echo "--- Environment Variables List ---"
printenv
echo -n "Total count of environment variables: "
printenv | wc -l
