#!/bin/bash

echo "PATH = $PATH"
echo "HOME = $HOME"
echo "USER = $USER"
echo "SHELL = $SHELL"

n=$(echo "$PATH" | tr ':' '\n' | grep -v '^$' | wc -l)
echo "Number of directories in PATH: $n"

m=$(printenv | wc -l)
echo "Number of environment variables: $m"

