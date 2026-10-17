#!/bin/bash

# Print environment variables
echo "$PATH"
echo "$HOME"
echo "$USER"
echo "$SHELL"

# Count directories in PATH
echo "$PATH" | tr ':' '\n' | wc -l

# List all environment variables
printenv

# Count environment variables
printenv | wc -l