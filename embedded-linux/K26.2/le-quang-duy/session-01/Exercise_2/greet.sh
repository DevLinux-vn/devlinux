#!/bin/bash

# Check if fewer than 2 arguments are provided
if [ "$#" -lt 2 ]; then
    echo "Error: Missing arguments."
    echo "Usage: $0 <name> <age>"
    exit 1
fi

# Print the requested output
echo "Hello, my name is $1 and I am $2 years old."
echo "Total arguments received: $#"