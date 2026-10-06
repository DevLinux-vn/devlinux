# IoT Environmental Monitoring System — Sprint Plan

## 1. Sprint Overview

### Sprint Goal

Deliver a complete IoT Environmental Monitoring System as a desktop C11 simulation.

The sprint covers the implementation, integration, testing, quality assurance, documentation, and release of the system.

The final system shall:

* Simulate temperature and humidity sensors.
* Process sensor data using a moving average filter.
* Detect configurable alert conditions.
* Serialize telemetry using TLV with CRC-8 validation.
* Record system events using the industrial logger and event log.
* Provide a CLI controlled by a finite state machine.
* Persist system configuration.
* Pass all four project quality gates.
* Provide complete software engineering documentation.
* Deliver a tagged `v1.0.0` release on the `main` branch.

### Sprint Duration

* **Duration:** 2 weeks
* **Working days:** 10 days
* **Sprint:** Sprint 1
* **Committed scope:** 42 Story Points
* **Release target:** `v1.0.0`

### Team Capacity

This project is implemented as an individual engineering sprint.

The available capacity is therefore one developer working across the required roles:

* Developer
* Software Architect
* Product Owner / Project Manager
* DevOps Engineer
* QA / Test Engineer

The sprint is intentionally aggressive, with 42 Story Points committed over 10 working days.

---

# 2. Product Backlog

| Ticket ID | Summary                                                              | Story Points | Priority    | Difficulty       |
| --------- | -------------------------------------------------------------------- | -----------: | ----------- | ---------------- |
| IOT-001   | Set up CMake project with quality gates & HAL simulation layer       |            3 | P1 — Must   | Easy             |
| IOT-002   | Implement variadic logging module with POSIX file output             |            3 | P1 — Must   | Easy             |
| IOT-003   | Implement sensor HAL interface, temperature & humidity drivers       |            5 | P1 — Must   | Easy-Medium      |
| IOT-004   | Implement ring buffer, moving average filter & event log             |            5 | P1 — Must   | Medium           |
| IOT-005   | Implement TLV telemetry serialization & deserialization              |            5 | P2 — Should | Medium           |
| IOT-006   | Implement system FSM & CLI command dispatcher                        |            8 | P2 — Should | Medium-Difficult |
| IOT-007   | Integrate all modules, config persistence & main loop                |            8 | P2 — Should | Difficult        |
| IOT-008   | Set up GitHub Actions CI, write architecture docs & release `v1.0.0` |            5 | P3 — Could  | Difficult        |
| **Total** |                                                                      |       **42** |             |                  |

---

# 3. Work Breakdown Structure

## 3.1 Sprint Structure

```text
Sprint 1 — IoT Monitoring System v1.0.0
│
├── Week 1
│   ├── IOT-001 — Project scaffolding + HAL
│   ├── IOT-002 — Industrial logger
│   ├── IOT-003 — Sensor drivers
│   ├── IOT-004 — Data structures
│   └── IOT-005 — Telemetry
│
└── Week 2
    ├── IOT-006 — FSM + CLI
    ├── IOT-007 — System integration
    └── IOT-008 — CI + documentation + release
```

## 3.2 Sprint Schedule

| Day | Ticket  | Focus                                                                     | Deliverable                             | SP |
| --- | ------- | ------------------------------------------------------------------------- | --------------------------------------- | -: |
| 1   | IOT-001 | CMake scaffolding, quality gates, HAL simulation and timer                | Build system green, HAL unit tests pass |  3 |
| 2   | IOT-002 | Logger macros, variadic functions and POSIX file I/O                      | Logger tested, file output verified     |  3 |
| 3   | IOT-003 | Sensor interface, opaque pointers, temperature and humidity drivers       | Polymorphic sensor read working         |  5 |
| 4   | IOT-004 | Ring buffer, moving average and intrusive linked-list event log           | Data structure unit tests pass          |  5 |
| 5   | IOT-005 | TLV serialization, endianness and safe parsing                            | Telemetry round-trip test passes        |  5 |
| 6–7 | IOT-006 | FSM state handlers, function-pointer table and CLI dispatcher             | FSM + CLI working end-to-end            |  8 |
| 8–9 | IOT-007 | Main loop, module wiring, configuration file and Design-by-Contract       | Full system runs as simulation          |  8 |
| 10  | IOT-008 | CI pipeline, architecture documentation, sprint documentation and release | All gates green, release complete       |  5 |

---

# 4. Week 1 Plan

## IOT-001 — Project Scaffolding & HAL

### Objectives

* Create the CMake project structure.
* Configure compiler warnings.
* Configure clang-tidy.
* Configure cppcheck.
* Configure sanitizers.
* Implement simulated hardware registers.
* Implement timer simulation.
* Create initial Unity tests.

### Expected result

The project builds successfully and HAL unit tests pass.

---

## IOT-002 — Industrial Logger

### Objectives

* Implement log levels.
* Implement logging macros.
* Implement variadic logging.
* Implement POSIX file output.
* Implement log-level filtering.
* Add Unity tests.

### Expected result

Logger output is correctly formatted, filtered and persisted to a file.

---

## IOT-003 — Sensor Drivers

### Objectives

* Define the sensor interface.
* Implement temperature driver.
* Implement humidity driver.
* Implement sensor manager.
* Apply opaque-pointer and function-pointer based abstraction.
* Add driver unit tests.

### Expected result

Temperature and humidity sensors can be accessed through a common sensor abstraction.

---

## IOT-004 — Data Processing

### Objectives

* Implement ring buffer.
* Implement moving average filtering.
* Implement event log.
* Use static memory management.
* Add unit tests for the data structures.

### Expected result

Sensor samples can be buffered, filtered and recorded as events.

---

## IOT-005 — Telemetry

### Objectives

* Implement TLV serialization.
* Implement TLV deserialization.
* Handle byte ordering.
* Implement CRC-8 validation.
* Validate input lengths and values.
* Add telemetry unit tests.

### Expected result

Telemetry data can be serialized and deserialized correctly with CRC validation.

---

# 5. Week 2 Plan

## IOT-006 — Application FSM & CLI

### Objectives

* Implement the five system states.
* Implement state handlers.
* Implement function-pointer state dispatch.
* Implement CLI command dispatcher.
* Validate state transitions.
* Add application-level tests.

### Expected states

```text
SYS_INIT
SYS_IDLE
SYS_MONITORING
SYS_ALERT
SYS_ERROR
```

### Expected result

The application can process CLI commands and transition between system states correctly.

---

## IOT-007 — System Integration

### Objectives

* Connect all modules.
* Implement the main application loop.
* Implement configuration persistence.
* Integrate sensor acquisition.
* Integrate filtering.
* Integrate alert checking.
* Integrate telemetry.
* Integrate logging.
* Apply Design-by-Contract checks.
* Run full-system tests.

### Expected result

The complete desktop simulation runs through the CLI and all major modules operate together.

---

## IOT-008 — CI, Documentation & Release

### Objectives

* Implement GitHub Actions CI.
* Configure four quality gates.
* Complete architecture documentation.
* Complete sprint documentation.
* Create `CHANGELOG.md`.
* Create `README.md`.
* Perform final quality verification.
* Create `release/v1.0.0`.
* Merge the release into `main`.
* Create the `v1.0.0` Git tag.

### Expected result

All quality gates pass and the project is released as `v1.0.0`.

---

# 6. Quality Gates

Every task must satisfy the four project quality gates.

## Gate 1 — Build & Clang-Tidy

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

The build must complete with zero compiler warnings.

Compiler options include:

```text
-Wall
-Wextra
-pedantic
-Werror
-std=c11
```

---

## Gate 2 — Pre-commit

```bash
pre-commit run --all-files
```

All configured pre-commit hooks must pass.

---

## Gate 3 — Static Analysis

```bash
cmake -B build
cmake --build build --target cppcheck
```

Cppcheck must complete without errors.

---

## Gate 4 — Sanitized Unit Tests

```bash
cmake -B build-asan \
    -DSANITIZER=asan+ubsan \
    -DCMAKE_BUILD_TYPE=Debug

cmake --build build-asan

ctest --test-dir build-asan -V --output-on-failure
```

All Unity test suites must pass under AddressSanitizer and UndefinedBehaviorSanitizer.

---

# 7. Definition of Done

A task is considered complete only when all of the following conditions are satisfied:

* [ ] Code compiles with zero warnings.
* [ ] `-Wall -Wextra -pedantic -Werror -std=c11` passes.
* [ ] All Unity unit tests pass.
* [ ] Unit tests pass under ASan + UBSan.
* [ ] `cppcheck` passes without errors.
* [ ] `clang-tidy` reports no warnings.
* [ ] `pre-commit run --all-files` passes.
* [ ] Doxygen comments are provided for public functions, structs and enums.
* [ ] BARR-C naming conventions are followed.
* [ ] Fixed-width integer types are used where appropriate.
* [ ] Mandatory braces are used.
* [ ] Commit messages contain the ticket ID.
* [ ] No unnecessary TODO/FIXME items remain.
* [ ] The feature branch is pushed to GitHub.
* [ ] The feature branch is merged into `develop` through a Pull Request.

---

# 8. Git Workflow

The project follows the required branch model:

```text
main
  │
  └── develop
        │
        ├── feature/IOT-001-*
        ├── feature/IOT-002-*
        ├── feature/IOT-003-*
        ├── feature/IOT-004-*
        ├── feature/IOT-005-*
        ├── feature/IOT-006-*
        ├── feature/IOT-007-*
        └── feature/IOT-008-*
```

At the end of the sprint:

```text
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

Feature branches are never merged directly into `main`.

---

# 9. Release Plan

The release process consists of the following steps:

1. Merge all completed feature branches into `develop`.
2. Confirm that `develop` is clean and all tests pass.
3. Create the release branch:

```bash
git checkout develop
git pull origin develop
git checkout -b release/v1.0.0
```

4. Run all four quality gates.
5. Push the release branch and verify GitHub Actions.
6. Merge `release/v1.0.0` into `main`.
7. Create the annotated release tag:

```bash
git tag -a v1.0.0 \
    -m "Release v1.0.0: IoT Monitoring System"
```

8. Push `main` and the tag:

```bash
git push origin main --tags
```

The final release must contain the `v1.0.0` tag on `main`.

---

# 10. Retrospective

## What Went Well

* Which implementation tasks were completed successfully?
* Which quality gates worked reliably?
* Which modules were easiest to integrate?
* Which parts of the Git workflow worked well?
* Was the project architecture clear and maintainable?

## What Could Be Improved

* Which tasks required more effort than estimated?
* Which implementation issues caused rework?
* Were there any integration problems between modules?
* Were any quality-gate failures difficult to resolve?
* Was the sprint workload realistic?

## Action Items

| Action                                    | Owner     | Priority | Status  |
| ----------------------------------------- | --------- | -------- | ------- |
| Improve CI feedback and diagnostics       | Developer | Medium   | Planned |
| Review architecture/dependency boundaries | Developer | Medium   | Planned |
| Improve automated test coverage           | Developer | Medium   | Planned |
| Review documentation after release        | Developer | Low      | Planned |

## Sprint Review

The sprint is considered successful when:

* All required modules are integrated.
* All four quality gates pass.
* GitHub Actions CI is green.
* Required SWE documentation is complete.
* The repository is public.
* `v1.0.0` is tagged on `main`.

---

# 11. Sprint Completion Checklist

* [ ] IOT-001 completed
* [ ] IOT-002 completed
* [ ] IOT-003 completed
* [ ] IOT-004 completed
* [ ] IOT-005 completed
* [ ] IOT-006 completed
* [ ] IOT-007 completed
* [ ] IOT-008 completed
* [ ] Four local quality gates pass
* [ ] GitHub Actions CI passes
* [ ] `ARCHITECTURE.md` completed
* [ ] `SPRINT_PLAN.md` completed
* [ ] `CHANGELOG.md` completed
* [ ] `README.md` completed
* [ ] `release/v1.0.0` created
* [ ] `release/v1.0.0` merged into `main`
* [ ] `v1.0.0` tag created
* [ ] `v1.0.0` pushed to GitHub
