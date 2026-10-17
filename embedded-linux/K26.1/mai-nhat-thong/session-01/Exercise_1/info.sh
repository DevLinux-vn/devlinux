#!/usr/bin/env bash

# Exit immediately if a command exits with a non-zero status,
# Treat unset variables as an error, and prevent errors in pipelines from being masked.
set -euo pipefail

# Print system information using standard environment variables
echo "Hello, ${USER}!"
echo "Your home directory is: ${HOME}"
echo "Your shell is: ${SHELL}"
echo "Current directory: ${PWD}"