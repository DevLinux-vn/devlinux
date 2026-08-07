#!/bin/bash

# Check if exactly 2 parameters are provided
if [ $# -lt 2 ]; then
    echo "Usage: $0 <name> <age>"
    echo "Example: $0 John 25"
    exit 1
fi

# Extract parameters
name=$1
age=$2

# Print greeting message
echo "Hello, my name is $name and I am $age years old."
echo "Total arguments received: $#"