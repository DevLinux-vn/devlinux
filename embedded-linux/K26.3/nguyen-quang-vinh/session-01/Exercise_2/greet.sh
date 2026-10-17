if [ "$#" -lt 2 ]; then
    echo "This file uses 2 input parameters"
    exit 1
else
    echo "Hello, my name is $1 and I am $2 years old."
    echo "Total arguments received: $#"
fi
