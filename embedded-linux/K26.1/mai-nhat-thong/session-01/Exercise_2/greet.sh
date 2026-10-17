#!/usr/bin/env bash
set -euo pipefail

# Function to display usage instructions
usage() {
    echo "Usage: $0 <name> <age>" >&2
    echo "Error: Invalid number of arguments." >&2
    exit 1
}

# Check if fewer than 2 parameters are provided
if [ "$#" -lt 2 ]; then
    usage
fi

# Assign positional parameters to local variables for readability
readonly NAME="$1"
readonly AGE="$2"
readonly TOTAL_ARGS="$#"

# Print the formatted output
echo "Hello, my name is ${NAME} and I am ${AGE} years old."
echo "Total arguments received: ${TOTAL_ARGS}"