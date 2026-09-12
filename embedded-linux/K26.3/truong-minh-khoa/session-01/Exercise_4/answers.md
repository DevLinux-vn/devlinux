
# Step 1 — create a regular variable
MY_NAME="Quan"
echo $MY_NAME        # visible?
# Answer of step1: The result is Quan. It is visible

# Step 2 — open a child shell and check
bash
echo $MY_NAME        # visible? why?
exit
# Answer of step2: The result is empty. It is invisible because the variable $MY_NAME only exist in parent shell and can't be inherited by child shell

# Step 3 — export the variable
export MY_NAME="Quan"
bash
echo $MY_NAME        # visible? why?
exit
# Answer of step3: The result is Quan. It is visible because the variable $MY_NAME has been exported to enviroment variables, so it can be seen by the child shell

# Step 4 — modify variable inside child shell
bash
MY_NAME="Alice"
echo $MY_NAME        # what do you see inside child shell?
exit
echo $MY_NAME        # back in parent shell — what do you see? why?
# Answer of step4: In child shell, the result of $MY_NAME is Alice. In parent shell, the result of $MY_NAME is Quan. The reason is that the MY_NAME in child shell is a clone of exported variable MY_NAME, not reference of exported variable MY_NAME, so the change of child shell's MY_NAME value had no the effect upon the output of MY_NAME in parent shell.
