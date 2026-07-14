echo $PATH
echo $HOME
echo $USER
echo $SHELL

echo $PATH | tr ':' '\n' | wc -l

printenv
printenv | wc -l