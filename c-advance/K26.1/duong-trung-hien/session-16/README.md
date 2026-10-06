<!-- FINAL_PROJECT_REPO: https://github.com/hien131106/iot_monitoring_system -->

# IoT Environmental Monitoring System

**Repository:** https://github.com/hien131106/iot_monitoring_system

A C11-based IoT Environmental Monitoring System designed as a desktop simulation of an embedded environmental monitoring application. The project demonstrates modular C programming, layered architecture, data processing, telemetry serialization, finite state machines, configuration persistence, unit testing, static analysis, sanitizers, and continuous integration.

## Overview

The system simulates an environmental monitoring device that reads temperature and humidity data, processes sensor measurements, detects alert conditions, records events, generates telemetry data, and provides a command-line interface for system control.

The project is implemented in C11 using CMake and follows a layered component architecture. Each component has a clearly defined responsibility and dependency relationship.

### Main capabilities

* Simulated temperature and humidity sensors.
* Common sensor interface with dedicated sensor drivers.
* Moving average processing using a ring buffer.
* Event logging with static memory management.
* TLV telemetry serialization with CRC-8 validation.
* Industrial logger with log-level filtering and POSIX file I/O.
* Configuration persistence with safe parsing and validation.
* Application finite state machine.
* CLI command dispatcher.
* Design-by-Contract checks.
* Unity unit tests integrated with CTest.
* Clang-Tidy static analysis.
* Cppcheck static analysis.
* Pre-commit quality checks.
* AddressSanitizer and UndefinedBehaviorSanitizer testing.
* GitHub Actions CI with four quality gates.

---

## Architecture

The project is organized into independent components with controlled dependencies.

```text
iot_monitoring_system/
├── app/
│   ├── CMakeLists.txt
│   └── main.c
│
├── components/
│   ├── common/
│   │   └── include/
│   │       ├── common_types.h
│   │       └── contract.h
│   │
│   ├── hal/
│   │   ├── include/
│   │   │   ├── hal_sim.h
│   │   │   └── hal_timer.h
│   │   └── src/
│   │       ├── hal_sim.c
│   │       └── hal_timer.c
│   │
│   ├── logger/
│   │   ├── include/
│   │   │   └── logger.h
│   │   └── src/
│   │       └── logger.c
│   │
│   ├── drivers/
│   │   ├── include/
│   │   │   ├── humi_drv.h
│   │   │   ├── sensor_intf.h
│   │   │   ├── sensor_mgr.h
│   │   │   └── temp_drv.h
│   │   └── src/
│   │       ├── humi_drv.c
│   │       ├── sensor_mgr.c
│   │       └── temp_drv.c
│   │
│   ├── data/
│   │   ├── include/
│   │   │   ├── data_proc.h
│   │   │   ├── event_log.h
│   │   │   └── ring_buffer.h
│   │   └── src/
│   │       ├── data_proc.c
│   │       ├── event_log.c
│   │       └── ring_buffer.c
│   │
│   ├── telemetry/
│   │   ├── include/
│   │   │   └── telemetry.h
│   │   └── src/
│   │       └── telemetry.c
│   │
│   ├── alert/
│   │   ├── include/
│   │   │   └── alert.h
│   │   └── src/
│   │       └── alert.c
│   │
│   ├── config/
│   │   ├── include/
│   │   │   └── config.h
│   │   └── src/
│   │       └── config.c
│   │
│   └── app/
│       ├── include/
│       │   ├── app_fsm.h
│       │   └── cli_cmd.h
│       └── src/
│           ├── app_fsm.c
│           └── cli_cmd.c
│
└── test/
    ├── test_app_logic.c
    ├── test_config.c
    ├── test_data_structures.c
    ├── test_hal_sim.c
    ├── test_logger.c
    ├── test_sensor_drv.c
    └── test_telemetry.c
```

Detailed architecture information and UML diagrams are available in:

`docs/ARCHITECTURE.md`

---

## Project Components

| Component   | Responsibility                                                        |
| ----------- | --------------------------------------------------------------------- |
| `common`    | Shared types, definitions, and Design-by-Contract support             |
| `hal`       | Simulated hardware registers and timer functionality                  |
| `logger`    | Log levels, logging macros, message formatting, and POSIX file output |
| `drivers`   | Temperature and humidity sensor drivers and sensor abstraction        |
| `data`      | Ring buffer, moving average processing, and event log                 |
| `telemetry` | TLV telemetry serialization and CRC-8 validation                      |
| `alert`     | Environmental alert processing                                        |
| `config`    | Configuration persistence and safe configuration parsing              |
| `app`       | Application FSM and CLI command dispatcher                            |
| `test`      | Unity-based unit tests executed through CTest                         |

---

## Requirements

The project requires:

* CMake 3.14 or newer
* GCC
* Git
* Python 3
* pre-commit
* Clang-Tidy
* Cppcheck

Unity is automatically downloaded by CMake through `FetchContent` when the test targets are configured.

On Ubuntu/Debian-based systems:

```bash
sudo apt update
sudo apt install cmake gcc clang-tidy cppcheck python3 python3-pip
```

Install pre-commit:

```bash
pip install pre-commit
```

---

## Build Instructions

Clone the repository:

```bash
git clone https://github.com/hien131106/iot_monitoring_system.git
cd iot_monitoring_system
```

Configure the Debug build:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
```

Build the project:

```bash
cmake --build build
```

The main executable is generated at:

```text
build/app/app
```

---

## Usage

Run the application:

```bash
./build/app/app
```

The application provides a command-line interface for controlling the monitoring system.

Available commands include:

```text
start
stop
alert_ack
reset
quit
```

A typical session starts the application and accepts commands through the CLI:

```text
=== IoT Environmental Monitoring System v1.0.0 ===
IoT> start
IoT> stop
IoT> start
IoT> alert_ack
IoT> reset
IoT> quit
```

The application reports its shutdown through the logger.

---

## Running Unit Tests

Configure and build the normal test configuration:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

Run all tests:

```bash
ctest --test-dir build -V --output-on-failure
```

The test suite covers:

* HAL simulation
* Logger
* Sensor drivers
* Data structures
* Telemetry
* Application logic
* Configuration

---

## Static Analysis

### Clang-Tidy

Clang-Tidy is integrated into the CMake build.

Build the project with:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

The project treats compiler warnings as errors through:

```text
-Wall
-Wextra
-pedantic
-Werror
-std=c11
```

### Cppcheck

The project provides a dedicated CMake target:

```bash
cmake -B build
cmake --build build --target cppcheck
```

The cppcheck target is configured to return a non-zero exit status when an analysis error is detected.

---

## Pre-commit

Run all configured pre-commit hooks:

```bash
pre-commit run --all-files
```

The repository currently uses automated formatting and static-analysis hooks, including:

* Clang-Format
* Clang-Tidy

All hooks must pass before changes are committed.

---

## Sanitizer Testing

The project supports AddressSanitizer and UndefinedBehaviorSanitizer through the `SANITIZER` CMake option.

Configure the sanitizer build:

```bash
cmake -B build-asan \
    -DSANITIZER=asan+ubsan \
    -DCMAKE_BUILD_TYPE=Debug
```

Build:

```bash
cmake --build build-asan
```

Run the complete test suite:

```bash
ctest --test-dir build-asan -V --output-on-failure
```

This configuration helps detect:

* Memory access violations
* Use-after-free errors
* Buffer overflows
* Undefined behavior
* Other runtime memory-related defects

---

## CI Pipeline

The project uses GitHub Actions to automatically verify the software quality.

The CI pipeline contains four quality stages:

### 1. Build & Clang-Tidy

The project is configured and compiled using CMake with the Debug configuration.

### 2. Pre-commit

All repository pre-commit hooks are executed.

### 3. Static Analysis

Cppcheck is executed through the project's CMake `cppcheck` target.

### 4. Unit Tests with Sanitizers

The project is built with:

```text
ASan + UBSan
```

and all CTest test suites are executed.

The workflow is located at:

```text
.github/workflows/ci.yml
```

---

## Quality Gates

All four quality gates must pass before a release:

```text
┌───────────────────────────────┐
│ Gate 1: Build + Clang-Tidy    │
└───────────────┬───────────────┘
                │
                ▼
┌───────────────────────────────┐
│ Gate 2: Pre-commit             │
└───────────────┬───────────────┘
                │
                ▼
┌───────────────────────────────┐
│ Gate 3: Cppcheck               │
└───────────────┬───────────────┘
                │
                ▼
┌───────────────────────────────┐
│ Gate 4: ASan + UBSan + CTest  │
└───────────────┬───────────────┘
                │
                ▼
             RELEASE
```

---

## Git Workflow

The project follows the feature branch workflow:

```text
feature/*
    │
    ▼
develop
    │
    ▼
release/v1.0.0
    │
    ▼
main
    │
    ▼
v1.0.0
```

Feature branches are developed independently and merged into `develop` through pull requests.

The final release is prepared from `develop` using:

```text
release/v1.0.0
```

After final validation, the release branch is merged into `main` and tagged:

```text
v1.0.0
```

---

## Release

The `v1.0.0` release represents the completed IoT Environmental Monitoring System with:

* Complete modular implementation
* Unit test coverage
* Static analysis
* Sanitizer testing
* Pre-commit checks
* GitHub Actions CI
* Architecture documentation
* Sprint documentation
* Release changelog

The release tag is:

```text
v1.0.0
```

---

## Documentation

Project documentation is located in the `docs/` directory.

### Architecture

`docs/ARCHITECTURE.md`

Contains:

* System architecture
* Use case diagram
* Component diagram
* Sequence diagram
* State diagram
* Module descriptions
* Module-to-lecture integration mapping

### Sprint Plan

`docs/SPRINT_PLAN.md`

Contains:

* Sprint goal
* Team capacity
* Product backlog
* Work Breakdown Structure
* Sprint schedule
* Definition of Done
* Retrospective template

### Changelog

`CHANGELOG.md`

Contains the release history following the Keep a Changelog format.

---

## Repository

**GitHub:** https://github.com/hien131106/iot_monitoring_system

<!-- FINAL_PROJECT_REPO: https://github.com/hien131106/iot_monitoring_system -->

---

## License

This project was developed as an academic final project for the DevLinux C Advanced course.
