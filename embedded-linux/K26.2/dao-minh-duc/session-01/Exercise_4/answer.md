# Shell Variables

## Step 1

Command:
`MY_NAME="Quan"; echo $MY_NAME`

Output: `Quan`

The variable is available in the current shell because it is a regular shell variable.

## Step 2

Command:
`bash; echo $MY_NAME; exit`

Output: empty.

A regular variable is not automatically inherited by a child shell because it was not exported.

## Step 3

Command:
`export MY_NAME="Quan"; bash; echo $MY_NAME; exit`

Output: `Quan`

`export` makes the variable available to child processes, so the child shell inherits it.

## Step 4

Inside child shell: `Alice`

After `exit`, in parent shell: `Quan`

The child shell has its own copy of the variable. Changes made in the child shell do not affect the parent shell.

## Summary

Regular variables are only available in the current shell. Exported variables are inherited by child processes. A child shell cannot change the parent's copy of a variable.
