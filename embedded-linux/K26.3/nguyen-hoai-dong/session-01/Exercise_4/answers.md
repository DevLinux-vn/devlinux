# Step 1 — create a regular variable
MY_NAME="Quan"
echo $MY_NAME        #Yes, it is visible

# Step 2 — open a child shell and check
bash
echo $MY_NAME        # It is not visible because it's not exist. MY_NAME in step 1 is a regular variable, and it only has scope in parent shell, not in child shell (like local vairable).
exit

# Step 3 — export the variable
export MY_NAME="Quan"
bash
echo $MY_NAME        # Yes, it is visible. After export, MY_NAME become a environment variable, that any shell can access it.
exit

# Step 4 — modify variable inside child shell
bash
MY_NAME="Alice"
echo $MY_NAME        # It prints Alice
exit
echo $MY_NAME        # back in parent shell — It prints Quan, because child shell inherited parent shell's environment variable, that like having a coppy, so can't affect the parent shell's environment variables