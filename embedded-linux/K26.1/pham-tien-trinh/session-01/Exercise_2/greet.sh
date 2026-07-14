#!/bin/bash

if [ $# -lt 2 ]; then
    echo "Usage: ./greet.sh <name> <age>"
    exit 1
fi

echo "Hello, my name is $1 and I am $2 years old."
echo "Total arguments received: $#"