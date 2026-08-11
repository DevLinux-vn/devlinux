# Step 1 — create a regular variable
MY_NAME="Quan"
echo $MY_NAME        # visible?

Yes

# Step 2 — open a child shell and check
bash
echo $MY_NAME        # visible? why?
exit

No, because the variable not passed to child shell

# Step 3 — export the variable
export MY_NAME="Quan"
bash
echo $MY_NAME        # visible? why?
exit

Yes, because after using export, the MY_NAME become global var in the current session

# Step 4 — modify variable inside child shell
bash
MY_NAME="Alice"
echo $MY_NAME        # what do you see inside child shell?
exit
echo $MY_NAME        # back in parent shell — what do you see? why?
```

In the child shell, it print Alice, but after exiting the child shell, it print Quan, because whatever change in child not affect global variable