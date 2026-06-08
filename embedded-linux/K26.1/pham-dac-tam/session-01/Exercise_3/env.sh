# Print PATH
echo $PATH

# Print HOME
echo $HOME

# Print USER
echo $USER

# Print SHELL
echo $SHELL

# Count directories in PATH
echo $PATH | tr ':' '\n' | wc -l

# List all environment variables
printenv

# Count all environment variables
printenv | wc -l