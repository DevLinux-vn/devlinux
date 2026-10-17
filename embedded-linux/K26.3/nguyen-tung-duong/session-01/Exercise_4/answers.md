# Answers

## Step 1

### Output

```text
Quan
```

### Explanation

`MY_NAME` is a regular shell variable. It is available in the current shell where it was created, so `echo $MY_NAME` prints `Quan`.

A regular variable is not automatically passed to the environment of child processes.

## Step 2

### Output

```text
```

### Explanation

The output is empty because `MY_NAME` is a regular shell variable and was not exported.

When a child shell is started, regular shell variables from the parent are not inherited by the child shell. Only variables that are exported as environment variables are passed to the child process.

## Step 3

### Output

```text
Quan
```

### Explanation

`MY_NAME` was exported using `export MY_NAME="Quan"`.

An exported variable becomes an environment variable, so it is inherited by child processes. Therefore, the child shell can access `MY_NAME` and `echo $MY_NAME` prints `Quan`.

## Step 4

### Output

Inside the child shell:

```text
Alice
```

After returning to the parent shell:

```text
Quan
```

### Explanation

The child shell inherits the exported variable `MY_NAME` with the value `Quan`. When `MY_NAME="Alice"` is executed inside the child shell, it changes the child's own copy of the variable.

This change does not affect the parent shell because the parent and child shell have separate copies of their shell variables. The child process cannot modify the variable stored in the parent shell.

Therefore, the child shell sees `Alice`, while the parent shell still sees `Quan`.

