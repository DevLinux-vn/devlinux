# Shell Variables and Subshells:

## Step 1: Create a regular variable
* **Visible?** Yes. 
* **See:**  `Quan`
* **Why:** `MY_NAME` is defined as a local variable in the current shell. The `echo` command easily reads it from the current shell's memory.

## Step 2: Open a child shell and check
* **Visible?** No (the output is blank).
* **Why:** By default, variables created in a shell are **local** to that specific process. When typing `bash` to open a new child shell, the parent shell does NOT pass its local variables to the child. Therefore, the child shell has no knowledge of `MY_NAME`.

## Step 3: Export the variable
* **Visible?** Yes, the output displays `Quan`.
* **Why:** The `export` command promotes `MY_NAME` from a local variable to an **environment variable**. When a parent process spawns a child process (the new `bash` shell), it passes a copy of all its exported environment variables to the child. 

## Step 4: Modify variable inside child shell
* **Inside child shell:** 
  * **What do you see?** `Alice`.
  * **Why?** Modified the value of `MY_NAME` in the local context of the child shell, overriding its inherited value.
* **Back in parent shell (after `exit`):** 
  * **What do you see?** `Quan`.
  * **Why?** Child processes receive a **copy** of the environment variables from the parent, not a reference to the original. Environment inheritance is strictly **one-way** (parent to child). Any modifications made inside the child shell only affect its own temporary copy and are completely destroyed when the child shell exits. The parent shell's original variable remains untouched.