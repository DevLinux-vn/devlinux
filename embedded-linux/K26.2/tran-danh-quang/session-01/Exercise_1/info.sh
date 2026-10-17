#!/bin/bash
# ------------------------------------------------------------------
# Script: info.sh
# Purpose: Prints basic information about the current user,
#          their home directory, default shell, and current directory
#          using built-in environment variables.
# ------------------------------------------------------------------

echo "Hello, $USER!"
echo "Your home directory is: $HOME"
echo "Your shell is: $SHELL"
echo "Current directory: $PWD"
