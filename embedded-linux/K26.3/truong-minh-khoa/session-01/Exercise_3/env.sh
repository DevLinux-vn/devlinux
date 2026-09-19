#!/bin/bash

#Print the value of `$PATH`, `$HOME`, `$USER`, `$SHELL`
echo "PATH=$PATH"
echo "HOME=$HOME"
echo "USER=$USER"
echo "SHELL=$SHELL"

# Split PATH by colon delimiter and replace with newline to count the total path with option -l of wc command
# grep -v '^$' to remove any existed blank line
echo "$PATH" | tr ':' '\n' | grep -v '^$' | wc -l

# Use `printenv` to list all environment variables and count how many there are with wc command and option -l for lines
printenv | wc -l


