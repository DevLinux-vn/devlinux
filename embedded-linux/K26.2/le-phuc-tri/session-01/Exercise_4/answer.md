# Step 1 — create a regular variable
MY_NAME="Quan"
echo $MY_NAME        # visible?

#Output:Quan
#Answer:visible because its a parent bash where the My_Name created
# Step 2 — open a child shell and check
bash
echo $MY_NAME        # visible? why?
exit

#Output:""(empty output)
#answer:No visible because MY_Name just a regular variable only available in the current shell where it created

# Step 3 — export the variable
export MY_NAME="Quan"
bash
echo $MY_NAME        # visible? why?
exit

#Output:Quan
#Explain: command "export" make My_Name an enviroment variables and it is inherited by children shell, so the child shell can access and printed
# Step 4 — modify variable inside child shell
bash
MY_NAME="Alice"
echo $MY_NAME        # what do you see inside child shell?
exit
echo $MY_NAME        # back in parent shell — what do you see? why?

#Output:
#inside child shell:Alice
#in the parent shell:Quan
#expain:Changing MY_NAME inside the child shell only affects that child shell. When the child shell exits, its changes are discarded. The parent shell keeps its original value because parent and child shells have separate environments.
