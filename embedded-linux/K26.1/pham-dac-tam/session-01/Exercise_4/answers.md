# Exercise 4 Answers

## Step 1

```bash
MY_NAME="Quan"
echo $MY_NAME
```

Output:

```text
Quan
```

Explanation:

`MY_NAME` is a regular shell variable created in the current shell process. It can be accessed only inside the shell where it was defined.

---

## Step 2

```bash
bash
echo $MY_NAME
```

Output:

```text
```

(empty)

Explanation:

A new Bash process (child shell) is created. Regular shell variables are not inherited by child processes, so `MY_NAME` is not available inside the child shell.

---

## Step 3

```bash
export MY_NAME="Quan"
bash
echo $MY_NAME
```

Output:

```text
Quan
```

Explanation:

`export` converts the shell variable into an environment variable. Environment variables are inherited by child processes, so the child shell can access `MY_NAME`.

---

## Step 4

```bash
bash
MY_NAME="Alice"
echo $MY_NAME
```

Output inside child shell:

```text
Alice
```

After exiting child shell:

```bash
echo $MY_NAME
```

Output in parent shell:

```text
Quan
```

Explanation:

The child shell has its own copy of the environment. Changing `MY_NAME` inside the child shell affects only that process. The parent shell keeps its original value.

---

# Summary

## Regular Variable

A regular variable exists only in the current shell process and is not inherited by child processes.

Example:

```bash
MY_NAME="Quan"
```

---

## Exported Variable

An exported variable becomes an environment variable and is inherited by child processes.

Example:

```bash
export MY_NAME="Quan"
```

---

## Why Child Changes Do Not Affect Parent

When a child shell is created, it receives a copy of the parent's environment. Any modifications made inside the child affect only the child process.

Parent process:

```text
MY_NAME = Quan
```

Child process:

```text
MY_NAME = Alice
```

After the child exits, the parent still has:

```text
MY_NAME = Quan
```

because processes in Linux are isolated and cannot directly modify the memory of their parent process.
