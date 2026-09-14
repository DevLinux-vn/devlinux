#!/bin/bash

# Display the requested system variables.
echo "===== System variables ====="
printf 'PATH=%s\n' "${PATH:-not set}"
printf 'HOME=%s\n' "${HOME:-not set}"
printf 'USER=%s\n' "${USER:-not set}"
printf 'SHELL=%s\n' "${SHELL:-not set}"

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
if ! environment_output=$(command -p printenv); then
    echo "Error: could not list environment variables." >&2
    exit 1
fi
printf '%s\n' "$environment_output"

echo
echo "===== Environment variable count ====="
# Count the same output that was displayed, and check the counting command.
if [ -z "$environment_output" ]; then
    echo 0
elif ! printf '%s\n' "$environment_output" | command -p wc -l; then
    echo "Error: could not count environment variables." >&2
    exit 1
fi
