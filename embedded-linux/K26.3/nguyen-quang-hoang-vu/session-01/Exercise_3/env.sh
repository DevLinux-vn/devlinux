#!/bin/bash
echo "PATH  = $PATH"
echo "HOME  = $HOME"
echo "USER  = $USER"
echo "SHELL = $SHELL"

echo "Number of directories in PATH:"
echo "$PATH" | tr ':' '\n' | wc -l

echo "Number of environment variables:"
printenv | wc -l
