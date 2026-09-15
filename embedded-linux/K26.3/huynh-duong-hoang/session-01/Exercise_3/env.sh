#!/bin/bash

echo "PATH: $PATH"
echo "HOME: $HOME"
echo "USER: $USER"
echo "SHELL: $SHELL"

echo "Number of directories in PATH:"
echo "$PATH" | tr ':' '\n' | wc -l

echo "Listing all environment variables:"
printenv

echo "Total number of environment variables:"
printenv | wc -l
