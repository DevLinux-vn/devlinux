# 1. Tree folder

```bash
<current directory>
├── include                       --- Headers
│   ├── mem_layout.h
│   └── ipv4_types.h
├── src                           --- Library source files
│   └── mem_layout.c
├── test                          --- Test files
│   └── integration_test          --- Integration test for the library
│           ├── main.c            --- Main test file
│           └── Makefile          --- Makefile for building the integration test
├── Makefile                      --- Makefile for building the library
└── README.md                     --- This file
```
# 2. Library Overview

Description of the library, its purpose, and how it can be used.

## 2.1 How to build the library

To build the library, run the following command in the *<current directory>*:

```bash
make all
```

The library will be built and the output file `libmem_layout.a` will be placed in the *<current directory>/lib/*.

## 2.2 How to remove the library

To remove the library from the *<current directory>/lib/* directory, run the following command in the *<current directory>*:

```bash
make clean
```

*<current directory>/lib/* will be removed, and the library will no longer be available for use.

## 2.3 How to use the library

To use the library in your project, include the header files from the *<current directory>/include/* directory and link against the `libmem_layout.a` library file.

# 3. Test Overview

## 3.1 How to build the integration test

To build the integration test, navigate to the *<current directory>/test/integration_test/* directory and run the following command:

```bash
make all
```

The integration test will be built and the output file `integration_test` will be placed in the *<current directory>/build/*. If the library is not built yet, the Makefile will automatically build the library before building the integration test.

## 3.2 How to remove the integration test

To remove the integration test from the *<current directory>/build/* directory, run the following command in the *<current directory>/test/integration_test/* directory:

```bash
make clean
```

The Makefile will remove *<current directory>/build/* and also remove the *<current directory>/lib/* directory if it was built as part of the integration test build process.

# 3.3 How to run the integration test

To run the integration test, navigate to the *<current directory>/build/* directory and execute the following command:

```bash
./integration_test <IPv4 address>
```

# 4. Static Analysis

How to perform static analysis on the library and integration test code using tools like `cppcheck` or `clang-tidy`. Include instructions for installing the tools, running the analysis, and interpreting the results.

## 4.1 How to run clang-tidy

Assuming you are in the *<current directory>*, run the following command to perform static analysis using `clang-tidy`:

```bash
clang-tidy src/*.c include/*.h -checks='clang-analyzer-*,cert-*,bugprone-*' -- -Iinclude
```

# 4.2 How to run cppcheck

Assuming you are in the *<current directory>*, run the following command to perform static analysis using `cppcheck`:

```bash
cppcheck --enable=all --inconclusive --std=c99 --language=c -Iinclude --suppress=missingIncludeSystem include/*.h src/*.c test/integration_test/main.c include
```