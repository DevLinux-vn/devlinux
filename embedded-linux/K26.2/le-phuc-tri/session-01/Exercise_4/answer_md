# Exercise 4 Answers

## Step 1

### Commands

```bash
MY_NAME="Quan"
echo $MY_NAME
```

### Output

```
Quan
```

### Explanation

`MY_NAME` is a regular shell variable. It is available only in the current shell where it was created.

---

## Step 2

### Commands

```bash
bash
echo $MY_NAME
exit
```

### Output

```

```

(empty output)

### Explanation

The child shell cannot access `MY_NAME` because it is a regular variable and has not been exported. Regular variables exist only in the current shell.

---

## Step 3

### Commands

```bash
export MY_NAME="Quan"
bash
echo $MY_NAME
exit
```

### Output

```
Quan
```

### Explanation

The `export` command makes `MY_NAME` an environment variable. Environment variables are inherited by child shells, so the child shell can access and print its value.

---

## Step 4

### Commands

```bash
bash
MY_NAME="Alice"
echo $MY_NAME
exit

echo $MY_NAME
```

### Output

Inside child shell:

```
Alice
```

Back in parent shell:

```
Quan
```

### Explanation

Changing `MY_NAME` inside the child shell only affects that child shell. When the child shell exits, its changes are discarded. The parent shell keeps its original value because parent and child shells have separate environments.

---

# Summary

## Difference between a regular variable and an exported variable

- A regular variable exists only in the current shell.
- An exported variable becomes an environment variable and is inherited by child shells.

## Why changes made inside a child shell do not affect the parent shell

A child shell is a separate process with its own copy of the environment. Any changes made inside the child shell remain local to that process. After the child shell exits, the parent shell is unchanged.
