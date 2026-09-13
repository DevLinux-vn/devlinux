#!/bin/bash

# Task 1: Print the value of $PATH, $HOME, $USER, $SHELL
echo "PATH: $PATH"     #Print the value of $PATH
echo "HOME: $HOME"     #Print the value of $HOME
echo "USER: $USER"     #Print the value of $USER        
echo "SHELL: $SHELL"   #Print the value of $SHELL 

# Task 2: Count how many directories are in $PATH
echo -n "Number of directories in $PATH: "
echo "$PATH" | tr ':' '\n' | wc -l

# Task 3: Use printenv to list all environment variables and count how many there are
echo "--- Environment Variables List ---"
printenv
echo -n "Total count of environment variables: "
printenv | wc -l
