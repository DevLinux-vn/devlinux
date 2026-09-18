 #!bin/bash

 echo "1. Print value of environment variables"
 echo "Path: $PATH"
 echo "Home: $HOME"
 echo "User: $USER"
 echo "Shell: $SHELL"
 echo ""

 echo "2. Count directories in Path"
 echo -n "Number of directories in Path: "
 echo "$PATH" | tr ":" "\n" | wc -l
 echo ""

 echo "3. Count number of environment variables"
 echo -n "Number of environment variables: "
 printenv | wc -l
 echo ""
