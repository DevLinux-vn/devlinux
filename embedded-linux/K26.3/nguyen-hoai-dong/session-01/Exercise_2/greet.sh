#!bin/bash

NAME=$1
AGE=$2
TOTAL_ARGUMENTS=$#

if [ $TOTAL_ARGUMENTS -lt 2 ]; then
	echo "Invalid arguments, please enter NAME and AGE"
else
	echo "Hello, my name is $NAME and I am $AGE years old."
        echo "Total arguments received: $TOTAL_ARGUMENTS"
fi
