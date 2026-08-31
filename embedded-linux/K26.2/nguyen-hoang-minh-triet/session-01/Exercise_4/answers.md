# Step 1 — create a regular variable
MY_NAME="Quan"
echo $MY_NAME 

visible?
Quan

why?
Because MY_NAME is a regular shell variable. It is available in the current shell.

# Step 2 — open a child shell and check
bash
echo $MY_NAME        
exit

visible? 
None data in child shell.

why?
Because A regular shell variable belongs only to the current shell. It is not automatically passed to child processes.

# Step 3 — export the variable
export MY_NAME="Quan"
bash
echo $MY_NAME        
exit

visible? 
Quan

why?
The variable is visible in the child shell because export makes MY_NAME an environment variable. Environment variables are inherited by child shells.

# Step 4 — modify variable inside child shell
bash
MY_NAME="Alice"
echo $MY_NAME        # what do you see inside child shell?
exit
echo $MY_NAME        # back in parent shell — what do you see? why?

Result inside the child shell:

Alice

Result in the parent shell:

Quan

The change to Alice only affects the copy of MY_NAME inside the child shell.

A child shell inherits variables from its parent, but changes made to those variables in the child shell are not propagated back to the parent shell.