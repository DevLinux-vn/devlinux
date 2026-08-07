#!/bin/bash

# Check if at least 2 arguments are provided
if [ $# -lt 2 ]; then
    echo "Usage: ./greet.sh <name> <age>"
    exit 1
fi

# Print greeting
echo "Hello, my name is $1 and I am $2 years old."

# Print total number of arguments
echo "Total arguments received: $#"
