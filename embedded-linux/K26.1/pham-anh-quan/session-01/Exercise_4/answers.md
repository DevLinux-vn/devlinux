# Exercise 4 - Understanding Linux Shell Variables

This report details the behavior of local shell variables versus environment variables across parent and child shell environments.

---

## 📊 Summary Table of Results

| Step | Scope Environment | Variable Value | Visibility | Core Reason |
| :--- | :--- | :--- | :--- | :--- |
| **Step 1** | Parent Shell | `Quan` | **Visible** | Local variable is accessible within its own process scope. |
| **Step 2** | Child Shell (`bash`) | *None* | **Not Visible** | Local variables are not inherited by child processes. |
| **Step 3** | Child Shell (`bash`) | `Quan` | **Visible** | `export` promotes the variable to the environment block for inheritance. |
| **Step 4** | Inside Child Shell | `Alice` | **Visible** | Local override inside the child process memory space. |
| **Step 4** | Return to Parent Shell | `Quan` | **Visible** | Child processes cannot modify the parent's environment block. |

---

## 🔍 Detailed Step-by-Step Analysis

### 🟦 Step 1 — Create a Regular Variable
* **Commands executed:**
```bash
  MY_NAME="Quan"
  echo $MY_NAME