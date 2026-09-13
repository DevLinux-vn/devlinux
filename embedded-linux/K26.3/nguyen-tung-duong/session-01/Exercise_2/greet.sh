#!/bin/bash
#If fewer than 2 parameters are provided, print usage instructions and exit with a non-zero exit code.
if [ $# -lt 2 ] ; then
	echo "usage: $0 <name> <age>"	
	exit 1
fi

NAME=$1
AGE=$2

echo "Hello. my name is $NAME and I am $AGE years old."
echo "Total arguments received: $#"

