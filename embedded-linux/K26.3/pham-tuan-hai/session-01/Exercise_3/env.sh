#!/bin/bash

echo "Path value: $PATH"
echo "Home value: $HOME"
echo "User value: $USER"
echo "Shell value: $SHELL"

echo "Count: $(echo $PATH | tr ':' '\n' | wc -l)"

printenv

echo "Count of environment variables: $(printenv | wc -l)"
