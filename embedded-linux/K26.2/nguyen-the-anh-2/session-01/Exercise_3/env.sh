#!/bin/bash

echo "Path: $PATH"
echo "Home: $HOME"
echo "User: $USER"
echo "Shell: $SHELL"
path_dir_count=$(echo "$PATH" | tr ':' '\n' | wc -l)
echo " Path count -----------------------------------"
echo "Co $path_dir_count thu muc trong $PATH."
echo ""

echo "All Environment Variables ----printenv"
printenv
echo ""

env_var_count=$(printenv | wc -l)
echo "Total number of environment variables: $env_var_count"