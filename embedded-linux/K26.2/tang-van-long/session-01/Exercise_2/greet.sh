#!/bin/bash

if [ "$#" -ge 2 ];
then
	echo "Hello, my name is \"$1\" and I am \"$2\" years old"
	echo "Total arguments received: $#"
else
	echo "Usage: Please enter 2 arguments: <name> <age>"
	exit 1
fi

exit 0
