#!/bin/bash

# 1. Print the values of PATH, HOME, USER, and SHELL
echo "=== Environment Variables ==="
echo "PATH: $PATH"
echo "HOME: $HOME"
echo "USER: $USER"
echo "SHELL: $SHELL"
echo ""

# 2. Count how many directories are in PATH
# Explanation: 'tr :' '\n'' replaces colons with newlines so each path is on its own line, 
# then 'wc -l' counts the total lines.
echo "=== Number of Directories in PATH ==="
echo "$PATH" | tr ':' '\n' | wc -l
echo ""

# 3. Use printenv to list all environment variables and count how many there are
echo "=== Total Environment Variables ==="
printenv | wc -l


=== Environment Variables ===
PATH: /home/triet/.vscode/extensions/stmicroelectronics.stm32cube-ide-core-1.4.0-linux-x64/resources/binaries/linux/x86_64:/home/triet/.vscode/extensions/stmicroelectronics.stm32cube-ide-build-cmake-1.46.0-linux-x64/resources/cube-cmake/linux/x86_64:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin:/snap/bin:/home/triet/.config/Code/User/globalStorage/github.copilot-chat/debugCommand
HOME: /home/triet
USER: triet
SHELL: /bin/bash

=== Number of Directories in PATH ===
13

=== Total Environment Variables ===
68
