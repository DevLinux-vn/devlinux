if [ "$#" -lt 2 ]; then
	echo "USAGE: $0 <name> <age>"
	exit 1
fi

echo "Hello, my name is $1 and I am $2 years old"
echo "Total arguments received: $#"
