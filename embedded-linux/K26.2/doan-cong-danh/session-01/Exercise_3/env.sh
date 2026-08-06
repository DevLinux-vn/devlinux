#!/bin/bash

echo "PATH: $PATH"
echo "HOME: $HOME"
echo "USER: $USER"
echo "SHELL: $SHELL"
echo "Directories in PATH: $(echo "$PATH" | tr ':' '\n' | wc -l)"
echo "Total environment variables: $(printenv | wc -l)"