#!/bin/bash
# Exercise_2 - greet.sh

if [ "$#" -lt 2 ]; then
    echo "Usage: $0 <name> <age>"
    exit 1
fi

name="$1"
age="$2"

if [[ ! "$name" =~ ^[A-Za-z]+$ ]]; then
    echo "Error: name must contain letters only (a-z, A-Z)."
    exit 2
fi

if [[ ! "$age" =~ ^[0-9]+$ ]]; then
    echo "Error: age must be a non-negative integer."
    exit 3
fi

echo "Hello, my name is ${name} and I am ${age} years old."
echo "Total arguments received: $#"
