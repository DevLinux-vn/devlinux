#!/bin/bash

# Use system variables; report missing or empty values without guessing.
printf 'Hello, %s!\n' "${USER:-not set}"
printf 'Your home directory is: %s\n' "${HOME:-not set}"
printf 'Your shell is: %s\n' "${SHELL:-not set}"
printf 'Current directory: %s\n' "${PWD:-not set}"
