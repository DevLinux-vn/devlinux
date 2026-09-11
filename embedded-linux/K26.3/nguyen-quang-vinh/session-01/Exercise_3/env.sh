#!/bin/bash

echo "PATH = $PATH"
echo "HOME = $HOME"
echo "USER = $USER"
echo "SHELL = $SHELL"

echo "Number of directories in PATH:"
echo "$PATH" | tr ':' '\n' | wc -l

echo "All environment variables:"
printenv

echo "Total environment variables:"
printenv | wc -l
