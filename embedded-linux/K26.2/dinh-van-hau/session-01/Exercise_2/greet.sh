#!/bin/bash

if [ $# -lt 2 ]; then
echo "You need provide 2 parameters"
exit 0
fi

echo "Hello, my name is $1 and I am $2 years old."
echo "Total arguments received: $#"
