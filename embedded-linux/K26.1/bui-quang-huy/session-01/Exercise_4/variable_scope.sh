#!/bin/bash

echo "============================================================"
echo " STEP 1 — Create a regular variable"
echo "============================================================"
MY_NAME="Quan"
echo "Parent shell: MY_NAME = $MY_NAME"
echo "Result: visible — it exists in the current shell process."

echo ""
echo "============================================================"
echo " STEP 2 — Open a child shell (variable NOT exported)"
echo "============================================================"
MY_NAME="Quan"
child_output=$(bash -c 'echo $MY_NAME')
if [ -z "$child_output" ]; then
    echo "Child shell:  MY_NAME = (empty)"
else
    echo "Child shell:  MY_NAME = $child_output"
fi
echo "Result: NOT visible — regular variables are local to the"
echo "        shell that defined them and are not inherited by"
echo "        child processes."

echo ""
echo "============================================================"
echo " STEP 3 — Export the variable, then open a child shell"
echo "============================================================"
export MY_NAME="Quan"
child_output=$(bash -c 'echo $MY_NAME')
echo "Child shell:  MY_NAME = $child_output"
echo "Result: visible — 'export' marks the variable to be passed"
echo "        into the environment of child processes."

echo ""
echo "============================================================"
echo " STEP 4 — Modify variable inside child shell"
echo "============================================================"
export MY_NAME="Quan"
echo "Parent shell before child: MY_NAME = $MY_NAME"

child_output=$(bash -c 'MY_NAME="Alice"; echo $MY_NAME')
echo "Inside child shell:        MY_NAME = $child_output"
echo "Parent shell after child:  MY_NAME = $MY_NAME"
echo "Result: child shell gets its OWN COPY of the variable."
echo "        Changes inside the child do NOT affect the parent."
echo "        Each process has its own isolated environment."
