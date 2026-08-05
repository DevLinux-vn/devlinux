### Step 1 — create a regular variable
```bash
$ MY_NAME="Quan"
$ echo $MY_NAME
Quan
```
**Explanation:** `MY_NAME` is visible in the current shell. This is because we just defined it as a local variable in the active shell.

### Step 2 — open a child shell and check
```bash
$ bash
$ echo $MY_NAME

$ exit
```
**Explanation:** `MY_NAME` is NOT visible (it prints empty). This is because `MY_NAME` is a regular (local) variable, and regular variables are not passed down or inherited by child processes (such as a newly opened bash shell).

### Step 3 — export the variable
```bash
$ export MY_NAME="Quan"
$ bash
$ echo $MY_NAME
Quan
$ exit
```
**Explanation:** `MY_NAME` is visible in the child shell. This is because we used the `export` command, which turns `MY_NAME` into an environment variable. Environment variables are automatically passed down (inherited) by all child processes.

### Step 4 — modify variable inside child shell
```bash
$ bash
$ MY_NAME="Alice"
$ echo $MY_NAME
Alice
$ exit
$ echo $MY_NAME
Quan
```
**Explanation:** Inside the child shell, `MY_NAME` prints as "Alice" because we overrode the value locally. Back in the parent shell (after exiting the child shell), `MY_NAME` is still "Quan". This happens because child processes receive a **copy** of the parent's environment variables. Any changes inside child shell (such as change variable, define new variable, etc.) only affect that copy and will disappear when the child shell is terminated. Child shell cannot change the parent shell's variables.
