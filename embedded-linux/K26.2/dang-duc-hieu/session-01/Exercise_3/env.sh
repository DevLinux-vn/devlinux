#!/bin/bash

echo "Path $PATH"
echo "Home $HOME"
echo "User $USER"
echo "Shell $SHELL"
echo "$PATH" | tr ':' '\n' | wc -l

echo "Total environment variables:"
printenv | wc -l