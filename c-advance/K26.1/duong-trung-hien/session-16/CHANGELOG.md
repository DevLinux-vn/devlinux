# Changelog

All notable changes to the IoT Environmental Monitoring System are documented in this file.

The format follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/), and the project follows Semantic Versioning.

## [1.0.0] - 2026-10-05

### Added

* IoT Environmental Monitoring System implemented in C11.
* Layered component architecture with common, HAL, logger, drivers, data, telemetry, alert, config, and application components.
* Simulated temperature and humidity hardware through the HAL.
* Temperature and humidity sensor drivers with a common sensor interface.
* Function-pointer based application FSM with five system states.
* CLI command dispatcher for system control.
* Moving average processing using a ring buffer.
* Event logging using an intrusive linked-list data structure and static memory management.
* TLV telemetry serialization with CRC-8 validation.
* Industrial logger with log-level filtering and POSIX file I/O.
* Persistent configuration with safe parsing and validation.
* Design-by-Contract checks.
* Unity unit tests integrated with CTest.
* Strict C11 compiler diagnostics with `-Wall`, `-Wextra`, `-pedantic`, and `-Werror`.
* Clang-Tidy static analysis.
* Cppcheck static analysis.
* Pre-commit quality checks.
* AddressSanitizer and UndefinedBehaviorSanitizer testing.
* GitHub Actions CI pipeline with four quality gates.
* Architecture and sprint planning documentation.
