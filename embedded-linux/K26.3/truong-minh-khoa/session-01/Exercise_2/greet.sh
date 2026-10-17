#!/bin/bash

AGE_PATTERN="^[0-9]+$"

if [ $# -lt 2 ]; then
    echo "Missing the second argument"
    echo "Usage:"
    echo "$0 <your-name> <age>"
    exit 1
fi

#if ! [[ $2 =~ $AGE_PATTERN ]]; then
if ! [[ $2 =~ ^[0-9]+$ ]]; then
    echo "Error: The age - second argument is not a valid number"
    exit 1
fi

echo "Hello, my name is $1 and I am $2 years old."
echo "Total arguments received: $#"
