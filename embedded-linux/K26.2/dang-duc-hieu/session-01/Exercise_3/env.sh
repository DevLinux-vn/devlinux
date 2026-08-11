echo "Path $PATH"
echo "Home $HOME"
echo "User $USER"
echo "Shell $SHELL"
echo directory count: "$PATH" | tr ':' '\n' | wc -l
printenv