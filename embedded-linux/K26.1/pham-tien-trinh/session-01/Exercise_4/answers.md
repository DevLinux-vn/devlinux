# Exercise 4 Answers

## Step 1

Command:

```bash
MY_NAME="Trinh"
echo $MY_NAME
```

Output:

```text
Trinh
```

Explanation:

`MY_NAME` is a regular shell variable created in the current shell. It is visible only inside the current shell session.

---

## Step 2

Command:

```bash
bash
echo $MY_NAME
exit
```

Output:

```text

```

(empty)

Explanation:

Running `bash` creates a child shell. Since `MY_NAME` was not exported, it is not inherited by the child shell. Therefore, the variable is not visible in the child shell.

---

## Step 3

Command:

```bash
export MY_NAME="Trinh"
bash
echo $MY_NAME
exit
```

Output:

```text
Trinh
```

Explanation:

The `export` command converts `MY_NAME` into an environment variable. Environment variables are inherited by child processes, so the child shell can access the variable.

---

## Step 4

Command inside child shell:

```bash
MY_NAME="Tien"
echo $MY_NAME
```

Output:

```text
Tien
```

After exiting the child shell:

```bash
echo $MY_NAME
```

Output:

```text
Trinh
```

Explanation:

The child shell receives a copy of the parent shell's environment. Modifying `MY_NAME` inside the child shell affects only the child shell. The parent shell keeps its original value because parent and child shells are separate processes.

---

## Difference Between a Regular Variable and an Exported Variable

- A regular variable exists only in the current shell.
- An exported variable becomes an environment variable and is inherited by child processes.

## Why Changes in a Child Shell Do Not Affect the Parent Shell

A child shell is a separate process. It receives a copy of the parent's environment. Any modifications made in the child shell affect only that copy and cannot change the environment of the parent shell.
