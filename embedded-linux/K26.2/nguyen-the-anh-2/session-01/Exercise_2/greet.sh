#!/bin/bash
if [ $# -lt 2 ]; then       
    echo "Usage: ./greet.sh <name> <age>"
    exit 1
fi
name=$1
age=$2
echo "Hello, my name is $name and I am $age years old."
echo "Total arguments received: $#"