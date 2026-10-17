# Exercise 4 - Variable Scope in Linux

## Objective

Understand the difference between:

* Shell Variable
* Environment Variable
* Parent Shell
* Child Shell

and observe how variables are inherited between processes.

---

## Step 1 - Create a Regular Variable

```bash
MY_NAME="Quan"
echo $MY_NAME
```

### Output

```text
Quan
```

### Explanation

A regular variable is created in the current shell.

At this moment:

```text
Parent Shell
└── MY_NAME = Quan
```

The variable exists only inside the current shell process.

---

## Step 2 - Open a Child Shell

```bash
bash
echo $MY_NAME
```

### Output

```text
```

(empty)

### Explanation

Running `bash` creates a new child shell.

```text
Parent Shell
└── Child Shell
```

The variable `MY_NAME` is not visible in the child shell because it is only a shell variable and has not been exported.

```text
Parent Shell
└── MY_NAME = Quan

Child Shell
└── MY_NAME does not exist
```

Therefore, `echo $MY_NAME` prints nothing.

---

## Step 3 - Export the Variable

Exit the child shell:

```bash
exit
```

Export the variable:

```bash
export MY_NAME="Quan"
```

Open another child shell:

```bash
bash
echo $MY_NAME
```

### Output

```text
Quan
```

### Explanation

The `export` command converts a shell variable into an environment variable.

Environment variables are inherited by child processes.

```text
Parent Shell
└── MY_NAME = Quan (exported)
      |
      +--> Child Shell
             └── MY_NAME = Quan
```

Because the variable is now part of the environment, the child shell can access it.

---

## Step 4 - Modify Variable Inside Child Shell

Inside the child shell:

```bash
MY_NAME="Alice"
echo $MY_NAME
```

### Output

```text
Alice
```

Exit the child shell:

```bash
exit
```

Back in the parent shell:

```bash
echo $MY_NAME
```

### Output

```text
Quan
```

### Explanation

When a child shell is created, it receives a copy of the parent's environment.

```text
Parent Shell
└── MY_NAME = Quan
      |
      +--> Child Shell
             └── MY_NAME = Quan
```

After changing the variable in the child shell:

```text
Parent Shell
└── MY_NAME = Quan

Child Shell
└── MY_NAME = Alice
```

The modification affects only the child shell's copy.

The parent shell remains unchanged.

This demonstrates an important rule:

> A child process can inherit variables from its parent, but it cannot modify variables in the parent process.

---

## Summary

| Action                  | Visible in Child Shell? | Reason                     |
| ----------------------- | ----------------------- | -------------------------- |
| `MY_NAME="Quan"`        | No                      | Shell variable only        |
| `export MY_NAME="Quan"` | Yes                     | Environment variable       |
| Modify in child shell   | Only child sees change  | Child has its own copy     |
| Return to parent shell  | Original value remains  | Child cannot modify parent |

---
