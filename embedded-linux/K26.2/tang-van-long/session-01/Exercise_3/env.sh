#!/bin/bash

echo "PATH = $PATH"
echo "HOME = $HOME"
echo "USER = $USER"
echo "SHELL = $SHELL"

n=$(echo "$PATH" | tr ':' '\n' | wc -l)
echo "Number of directories in PATH: $n"

m=$(printenv | wc -l)
printenv
echo "Number of emvironment variables: $m"

