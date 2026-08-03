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

# Count how many directories are in $PATH
echo -n "Number of directories in PATH: "
echo "$PATH" | tr ':' '\n' | wc -l

# Use printenv to list all environment variables and count how many there are
echo -n "Total environment variables: "
printenv | wc -l
