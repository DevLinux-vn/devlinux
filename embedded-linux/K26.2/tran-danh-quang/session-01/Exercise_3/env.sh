#!/bin/bash

# Print the value of $PATH, $HOME, $USER, $SHELL
echo "--- Environment Variables ---"
echo "PATH: $PATH"
echo "HOME: $HOME"
echo "USER: $USER"
echo "SHELL: $SHELL"
echo "-----------------------------"

# Count how many directories are in $PATH
echo -n "Number of directories in PATH: "
echo "$PATH" | tr ':' '\n' | wc -l

# Use printenv to list all environment variables and count how many there are
echo -n "Total environment variables: "
printenv | wc -l
