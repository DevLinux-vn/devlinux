#!/bin/bash
# ------------------------------------------------------------------
# Script: env.sh
# Purpose: Demonstrates working with environment variables,
#          printing specific ones, counting directories in PATH, and 
#          counting total environment variables in the system.
# ------------------------------------------------------------------

# 1. Print variables: $PATH, $HOME, $USER, $SHELL
echo "--- Environment Variables ---"
echo "PATH: $PATH"
echo "HOME: $HOME"
echo "USER: $USER"
echo "SHELL: $SHELL"
echo "-----------------------------"

echo
# 2. Count how many directories are in $PATH
# (Use -n to print the label on the same line as the count result)
echo -n "Number of directories in PATH: "
echo "$PATH" | tr ':' '\n' | wc -l

echo
# 3. Use printenv to list all environment variables and count how many there are
# (Use -n to print the label on the same line as the count result)
echo -n "Total environment variables: "
printenv | wc -l
