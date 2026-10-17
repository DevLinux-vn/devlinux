# Exercise 4 — Answers

## Step 1: Regular variable
Command: MY_NAME="VuNguyen" then echo $MY_NAME
Result: VuNguyen
Explanation: The variable is created and visible in the current shell.

## Step 2: Child shell (not exported)
Result: (empty)
Explanation: A regular variable is NOT inherited by a child shell,
so it is empty inside the child bash.

## Step 3: After export
Command: export MY_NAME="VuNguyen", then bash, then echo $MY_NAME
Result: VuNguyen
Explanation: export makes the variable an environment variable,
which is inherited by any child shell.

## Step 4: Modify inside child shell
- Inside child shell (MY_NAME="Alice"): Alice
- Back in parent shell: VuNguyen
Explanation: The child shell gets its own copy of the variable.
Changes made in the child do not affect the parent shell.

## Summary
- Regular variable: only exists in the current shell, not inherited by children.
- Exported variable: becomes an environment variable, inherited by child shells.
- A child shell runs in a separate process with copies of exported variables,
  so modifications inside it never propagate back to the parent.