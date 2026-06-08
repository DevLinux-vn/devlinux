#print PATH
echo $PATH

#Print HOME
echo $HOME

#print USER
echo $USER

#print SHELL
echo $SHELL

#count directories in PATH
echo $PATH | tr ':' '\n' | wc -l

#list all environment variables
printenv

#count all environment variables
printenv | wc -l
