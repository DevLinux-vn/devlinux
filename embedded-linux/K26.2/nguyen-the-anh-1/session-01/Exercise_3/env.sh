

# Command:
# echo "$PATH"

# Output:
# /usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin


# Command:
# echo "$HOME"

# Output:
# /home/theanh



# Command:
# echo "$USER"

# Output:
# theanh


# Command:
# echo "$SHELL"

# Output:
# /bin/bash

# Command:
# echo "$PATH" | tr ':' '\n' | wc -l

# Output:
# 9


# Command:
# printenv

# Output:
# SHELL=/bin/bash
#PWD=/home/theanh/devlinux/embedded-linux/K26.2/nguyen-the-anh-1/session-01
#LOGNAME=theanh
#XDG_SESSION_TYPE=tty
#HOME=/home/theanh
#LANG=en_US.UTF-8
#LS_COLORS=rs=0:di=01
#SSH_CONNECTION=192.168.188.1 61134 192.168.188.128 22
#LESSCLOSE=/usr/bin/lesspipe %s %s
#XDG_SESSION_CLASS=user
#TERM=xterm
#LESSOPEN=| /usr/bin/lesspipe %s
#USER=theanh
#DISPLAY=localhost:10.0
#SHLVL=1
#XDG_SESSION_ID=4
#XDG_RUNTIME_DIR=/run/user/1000
#SSH_CLIENT=192.168.188.1 61134 22
#DEBUGINFOD_URLS=https://debuginfod.ubuntu.com
#IM_CONFIG_ENTRY=profile
#XDG_DATA_DIRS=/usr/share/gnome:/usr/local/share:/usr/share:/var/lib/snapd/desktop
#PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin
#DBUS_SESSION_BUS_ADDRESS=unix:path=/run/user/1000/bus
#SSH_TTY=/dev/pts/0
#OLDPWD=/home/theanh/devlinux/embedded-linux/K26.2/nguyen-the-anh-1
#_=/usr/bin/printenv


# Command:
# printenv | wc -l

# Output:
# 26
