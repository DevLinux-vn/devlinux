#!/bin/zsh

#Print the value of `$PATH`, `$HOME`, `$USER`, `$SHELL`
echo "PATH=$PATH" && echo "HOME=$HOME" && echo "USER=$USER" && echo "SHELL=$SHELL"

# Count how many directories are in `$PATH`
echo $PATH | tr ":" "\n" | wc -l

# Use `printenv` to list all environment variables and count how many there are
printenv | wc -l


