#!/bin/bash

# Count directories in PATH
echo "Number of directories in PATH:"
echo "$PATH" | tr ':' '\n' | wc -l

# Print environment variables
echo "PATH=$PATH"
echo "HOME=$HOME"
echo "USER=$USER"
echo "SHELL=$SHELL"

# Count environment variables
echo "Number of environment variables:"
printenv | wc -l