#!bin/bash

echo $PATH
echo $HOME
echo $USER
echo $SHELL

STRING=$(echo $PATH | tr ':' '\n')
COUNT_DIRECTORIES=$(echo "$STRING" | wc -l)
echo $COUNT_DIRECTORIES

COUNT_VARIABLE=$(printenv | wc -l)
echo "COUNT VARIABLE:  $COUNT_VARIABLE"
