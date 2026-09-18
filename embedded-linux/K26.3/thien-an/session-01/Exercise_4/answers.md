# Exercise 4: Variable Scope and Environment

## Observations & Explanations

**Step 1: Create a regular variable**
* **Result:** I can see the name "Quan" printed.
* **Why:** `MY_NAME="Quan"` creates a regular (local) shell variable within the current shell process. The `echo` command successfully reads and prints it.

**Step 2: Open a child shell and check**
* **Result:** The variable is NOT visible (prints an empty line).
* **Why:** By default, regular shell variables are strictly local to the shell that created them. When we type `bash` to start a new child process, the child shell does NOT inherit regular variables from its parent.

**Step 3: Export the variable**
* **Result:** The name "Quan" is visible again inside the child shell.
* **Why:** The `export` command promotes a regular variable into an **environment variable**. Operating systems pass a copy of all environment variables to any new child processes created. Therefore, the child shell inherits `MY_NAME`.

**Step 4: Modify variable inside child shell**
* **Result:** 
  - Inside the child shell, it prints "Alice".
  - Back in the parent shell (after `exit`), it prints "Quan".
* **Why:** When a child shell inherits environment variables, it receives a **COPY** of those variables (similar to "pass-by-value" in C/C++). Changing `MY_NAME` to "Alice" only modifies the child's local copy. Once we `exit`, the child process is destroyed along with its copy, and we return to the parent shell where the original `MY_NAME` remains unchanged as "Quan".
