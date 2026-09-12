#! /bin/zsh

if [[ -z $2 ]]; then
    echo "Missing the second argument"
    echo "Usage:"
    echo "$SHELL $0 <your-name> <age>"
    exit 1
fi

echo "Hello, my name is $1 and I am $2 years old."
echo "Total arguments received: $#"
