# Assignment — Session 01 — Exercise 4 Answers
## Step-by-Step Observation and Explanation
### Step 1 — Create a regular variable
* **Command:**

```bash
MY_NAME="Quan"
echo $MY_NAME
```

* **Visibility:** Yes, the value Quan is displayed.
* **Why:** The variable MY_NAME is registered in the local memory space of the current shell process (the parent shell).
### Step 2 — Open a child shell and check
* **Command:**

```bash
echo $MY_NAME
```

* **Visibility:** No, it prints an empty line (blank).
* **Why:** MY_NAME is currently a local shell variable. In Linux, when a child process (bash) is forked, it only inherits the environment variables of the parent process, not the local ones. The child shell has its own isolated memory space.
### Step 3 — Export the variable
* **Command:**

```bash
export MY_NAME="Quan"
bash
echo $MY_NAME
```

* **Visibility:** Yes, the value Quan is visible inside the child shell.
* **Why:** The export command moves the variable from the local shell scope into the Environment Block of the process. When the child bash is spawned, the OS kernel copies this Environment Block into the address space of the child process, making it accessible.
### Step 4 — Modify variable inside child shell
* **Command:**

```bash
MY_NAME="Alice"
echo $MY_NAME  # Inside child shell -> Prints "Alice"
exit
echo $MY_NAME  # Back in parent shell -> Prints "Quan"
```

* **Visibility & Differences:**
    * Inside the child shell, MY_NAME becomes Alice.
    * After exit, back in the parent shell, MY_NAME remains Quan.
* **Why:** 
    * Inside Child: The child shell modifies its own local copy of the environment variable.
    * In Parent: Linux enforces strict memory isolation between processes. A child process is created via the fork() and exec() system calls, receiving a copy of the parent's environment. It has absolutely no privilege or mechanism to modify the memory segment of its parent process. Therefore, changes in the child are volatile and disappear when the child process terminates (exit).