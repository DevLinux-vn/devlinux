Exercise 4 - Shell Variables
Step 1 - Create a Regular Variable

Command:

MY_NAME="QUAN"

Then:

echo $MY_NAME

Output:

QUAN

Explanation:

The variable MY_NAME is visible because it was created in the current shell. At this point, MY_NAME is a regular shell variable. A regular variable can be accessed in the shell where it was created, but it is not automatically passed to a child shell.

Step 2 - Open a Child Shell and Check

After running bash, a new child shell is created.

Then I ran:

echo $MY_NAME

The output was empty.

Explanation:

MY_NAME was created as a regular variable in the parent shell and was not exported. Therefore, the child shell did not inherit this variable.

A regular variable exists only in the current shell unless it is exported.

After checking the variable, I used exit to return to the parent shell.

Step 3 - Export the Variable

I used:

export MY_NAME="Quan"

Then I opened a new child shell with bash and ran:

echo $MY_NAME

Output:

Quan

Explanation:

The export command makes MY_NAME an exported variable. Exported variables are passed from the parent shell to child processes.

Therefore, when I opened a new Bash shell, the child shell inherited MY_NAME from the parent shell and its value was Quan.

I also tried:

export MY_NAME = "Quan"

and received:

bash: export: '=': not a valid identifier

This happened because Bash does not allow spaces around the = sign when assigning a variable.

Step 4 - Modify the Variable Inside the Child Shell

Inside the child shell, I changed the variable using:

MY_NAME="Alice"

Then I ran:

echo $MY_NAME

Output:

Alice

Explanation:

The child shell initially inherited MY_NAME="Quan" from the parent shell. When I changed MY_NAME to Alice, the change only happened inside the child shell.

I then used exit to return to the parent shell and ran:

echo $MY_NAME

Output:

Quan

The parent shell still had Quan because the parent shell and child shell are separate processes. The child receives a copy of the exported environment variables from the parent. Changes made to the child's variables do not change the variables in the parent shell.
