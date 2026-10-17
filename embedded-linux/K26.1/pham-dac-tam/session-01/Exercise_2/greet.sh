#!/bin/bash

# Exercise 2 — Script with Parameters
# Greet with name and age, accept parameters

if [[ $# -lt 2 ]]; then
    echo "Usage: $0 <name> <age>"
    echo "Example: $0 John 25"
    exit 1
fi

name=$1
age=$2

echo "Hello, my name is $name and I am $age years old."
echo "Total arguments received: $#"
