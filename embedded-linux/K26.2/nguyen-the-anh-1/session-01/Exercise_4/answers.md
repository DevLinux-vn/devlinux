# Exercise 4 — Shell Variables

## Step 1 — Regular variable

Command:

MY_NAME="Quan"
echo $MY_NAME

Result:

Quan

Explanation:

The regular variable MY_NAME is visible in the current shell because it is created in that shell. However, it is not exported to child shells.

## Step 2 — Child shell with a regular variable

Command:

bash
echo $MY_NAME
exit

Result:

The output is empty.

Explanation:

A regular shell variable is only available in the current shell. It is not automatically inherited by a child shell. Therefore, the child shell cannot see MY_NAME.

## Step 3 — Export the variable

Command:

export MY_NAME="Quan"
bash
echo $MY_NAME
exit

Result:

Quan

Explanation:

The export command makes MY_NAME an environment variable. Exported variables are inherited by child processes, so the child shell can access MY_NAME.

## Step 4 — Modify the variable inside the child shell

Command:

bash
MY_NAME="Alice"
echo $MY_NAME
exit
echo $MY_NAME

Result inside the child shell:

Alice

Result in the parent shell:

Quan

Explanation:

The child shell receives its own copy of the exported variable. Changing MY_NAME to Alice inside the child shell only changes the child's copy. It does not change the variable in the parent shell. Therefore, after returning to the parent shell, MY_NAME is still Quan.

## Summary

A regular variable is available only in the current shell and is not automatically inherited by child shells.

An exported variable is passed to child processes, so child shells can access it.

However, the child shell has its own copy of the variable. Therefore, changes made inside a child shell do not affect the parent shell.
