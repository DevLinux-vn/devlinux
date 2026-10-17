 #!/bin/bash

echo "=== PATH ==="
echo "$PATH"

echo "=== HOME ==="
echo "$HOME"

echo "=== USER ==="
echo "$USER"

echo "=== SHELL ==="
echo "$SHELL"

echo "=== Number of directories in PATH ==="
echo "$PATH" | tr ':' '\n' | wc -l

echo "=== All environment variables ==="
printenv

echo "=== Number of environment variables ==="
printenv | wc -l
