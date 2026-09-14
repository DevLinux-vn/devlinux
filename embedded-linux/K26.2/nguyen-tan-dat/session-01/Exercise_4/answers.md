# Step 1 — create a regular variable
It's visible

# Step 2 — open a child shell and check
Not visible, because MY_NAME was never exported — the child process doesn't inherit non-exported shell variables at all.

# Step 3 — export the variable
Visible, because export puts it into the environment, and every child process gets a copy of the parent's environment at creation time.

# Step 4 — modify variable inside child shell
Inside child: Alice
Back in parent: Quan
Because the child received its own copy of the environment variable. Reassigning it inside the child only changes that child's local copy — child processes cannot modify variables in their parent's shell. When the child exits, its copy is discarded.
