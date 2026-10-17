#!/bin/bash


# check the parameter

if [ $# -lt 2 ]; then 
	echo "Usage: $0 <name> <age>"
	exit 1 
fi


# get the parameter


name=$1
age=$2

echo "Hello, my name is $name and I am $age years old."
echo "Total arguments received: $#"
