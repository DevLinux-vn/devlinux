#!/bin/bash

# Display the requested system variables.
echo "===== System variables ====="
echo "PATH=${PATH-}"
echo "HOME=$HOME"
echo "USER=$USER"
echo "SHELL=$SHELL"

echo
echo "===== PATH directory count ====="
if [ -z "${PATH-}" ]; then
    echo "PATH is empty or not set"
    echo "Number of directories in PATH: 0"
else
    # Count separators with Bash so unusual characters do not affect the count.
    remaining_path=$PATH
    directory_count=1
    while [[ "$remaining_path" == *:* ]]; do
        remaining_path=${remaining_path#*:}
        directory_count=$((directory_count + 1))
    done
    # Empty entries within PATH refer to the current directory.
    echo "Number of directories in PATH: $directory_count"
fi

echo
echo "===== Environment variables ====="
# Use the standard utility search path even if the user's PATH is empty.
command -p printenv

echo
echo "===== Environment variable count ====="
command -p printenv | command -p wc -l
