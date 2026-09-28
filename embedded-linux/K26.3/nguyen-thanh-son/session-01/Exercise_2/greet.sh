#!/bin/bash

if [ "$#" -lt 2 ]; then
    echo "Usage: $0 <name> <age>" >&2
    exit 1
fi

echo "Hello, my name is $1 and I am $2 years old."
echo "Total arguments received: $#"
echo