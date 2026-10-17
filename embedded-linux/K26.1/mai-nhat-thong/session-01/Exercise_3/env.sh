#!/usr/bin/env bash
set -euo pipefail

echo "========================================="
echo "1. Standard Environment Variables"
echo "========================================="
echo "echo \$PATH   -> ${PATH}"
echo "echo \$HOME   -> ${HOME}"
echo "echo \$USER   -> ${USER}"
echo "echo \$SHELL  -> ${SHELL}"
echo

echo "========================================="
echo "2. Directory count in \$PATH"
echo "========================================="
# tr replaces ':' with newlines, wc -l counts the lines
# Using printf to handle cases where PATH doesn't end with a newline safely
path_count=$(printf "%s" "$PATH" | tr ':' '\n' | wc -l)
echo "Command: printf \"%s\" \"\$PATH\" | tr ':' '\n' | wc -l"
echo "Result: ${path_count} directories in \$PATH"
echo

echo "========================================="
echo "3. Total Environment Variables (printenv)"
echo "========================================="
# printenv lists all vars, wc -l counts them
env_count=$(printenv | wc -l)
echo "Command: printenv | wc -l"
echo "Result: Total of ${env_count} environment variables active."
echo "========================================="