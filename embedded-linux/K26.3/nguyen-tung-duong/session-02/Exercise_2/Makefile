CUR_DIR := $(shell pwd)
INCLUDE_DIR := $(CUR_DIR)/include
SRC_DIR := $(CUR_DIR)/src
APP_DIR := $(CUR_DIR)/app
BUILD_DIR := $(CUR_DIR)/build
BIN_DIR := $(CUR_DIR)/bin
LOGS_DIR := $(CUR_DIR)/logs
SESSIONS_DIR := $(LOGS_DIR)/sessions

# Compiler and tools
CC := gcc

# Compiler flags
CFLAGS := -Wall -Wextra -I$(INCLUDE_DIR)
FPIC_FLAGS := -fPIC

# Library and executable files
LIBNAME := libstringutils.so
SHARED_LIB := $(BIN_DIR)/$(LIBNAME)
TARGET := $(BIN_DIR)/main

# Source and object files
LIB_OBJS := $(BUILD_DIR)/stringutils.o
APP_OBJS := $(BUILD_DIR)/main.o

# Generate timestamp for logging
TIMESTAMP := $(shell date +"%Y%m%d_%H%M%S")
BUILD_LOG := $(LOGS_DIR)/build_$(TIMESTAMP).log
RUN_LOG := $(LOGS_DIR)/run_$(TIMESTAMP).log
SESSION_DIR := $(SESSIONS_DIR)/session_$(TIMESTAMP)

# Ensure directories exist
_unused := $(shell mkdir -p $(BUILD_DIR) $(BIN_DIR) $(LOGS_DIR) $(SESSIONS_DIR))

# Default target
all: $(TARGET)
	@echo "✓ Build completed successfully at $(shell date '+%Y-%m-%d %H:%M:%S')" | tee -a $(BUILD_LOG)

# Create shared library
$(SHARED_LIB): $(LIB_OBJS)
	@echo "[BUILD] Creating shared library..." | tee -a $(BUILD_LOG)
	$(CC) -shared -o $@ $^ 2>&1 | tee -a $(BUILD_LOG)
	@echo "  ✓ Shared library created: $(SHARED_LIB)" | tee -a $(BUILD_LOG)

# Compile library source with PIC
$(BUILD_DIR)/stringutils.o: $(SRC_DIR)/stringutils.c
	@echo "[BUILD] Compiling (PIC): $<" | tee -a $(BUILD_LOG)
	$(CC) $(CFLAGS) $(FPIC_FLAGS) -c $< -o $@ 2>&1 | tee -a $(BUILD_LOG)

# Compile main program
$(BUILD_DIR)/main.o: $(APP_DIR)/main.c
	@echo "[BUILD] Compiling: $<" | tee -a $(BUILD_LOG)
	$(CC) $(CFLAGS) -c $< -o $@ 2>&1 | tee -a $(BUILD_LOG)

# Link executable with shared library
$(TARGET): $(APP_OBJS) $(SHARED_LIB)
	@echo "[BUILD] Linking: $@" | tee -a $(BUILD_LOG)
	$(CC) $(CFLAGS) -o $@ $(APP_OBJS) -L$(BIN_DIR) -lstringutils 2>&1 | tee -a $(BUILD_LOG)
	@echo "  ✓ Executable created: $(TARGET)" | tee -a $(BUILD_LOG)
	@echo "[BUILD] Verifying library dependencies..." | tee -a $(BUILD_LOG)
	@ldd $(TARGET) 2>&1 | tee -a $(BUILD_LOG)

# Run the application
run: $(TARGET)
	@mkdir -p $(SESSION_DIR)
	@echo "" | tee -a $(RUN_LOG)
	@echo "========================================" | tee -a $(RUN_LOG)
	@echo "Run started at $(shell date '+%Y-%m-%d %H:%M:%S')" | tee -a $(RUN_LOG)
	@echo "========================================" | tee -a $(RUN_LOG)
	@echo "Running: $(TARGET) \"Hello World From Shared Library!\"" | tee -a $(RUN_LOG)
	@echo "========================================" | tee -a $(RUN_LOG)
	@LD_LIBRARY_PATH=$(BIN_DIR):$$LD_LIBRARY_PATH $(TARGET) "Hello World From Shared Library!" 2>&1 | tee -a $(RUN_LOG)
	@echo "========================================" | tee -a $(RUN_LOG)
	@echo "Run completed at $(shell date '+%Y-%m-%d %H:%M:%S')" | tee -a $(RUN_LOG)
	@echo "========================================" | tee -a $(RUN_LOG)
	@echo ""
	@echo "[SESSION] Consolidating logs to: $(SESSION_DIR)"
	@cp $(LOGS_DIR)/build_$(TIMESTAMP).log $(SESSION_DIR)/ 2>/dev/null || true
	@cp $(LOGS_DIR)/run_$(TIMESTAMP).log $(SESSION_DIR)/ 2>/dev/null || true
	@cp $(LOGS_DIR)/clean_$(TIMESTAMP).log $(SESSION_DIR)/ 2>/dev/null || true
	@echo "[SESSION] All logs saved to session folder"
	@echo "✓ Session path: $(SESSION_DIR)"

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo "[CLEAN] Build artifacts removed at $(shell date '+%Y-%m-%d %H:%M:%S')" | tee -a $(LOGS_DIR)/clean_$(TIMESTAMP).log

# View build logs
logs:
	@echo "Available logs in $(LOGS_DIR):"
	@ls -lhtr $(LOGS_DIR)/ 2>/dev/null | grep -v "^d" || echo "No logs found yet"

# View latest build log
log-build:
	@tail -50 $$(ls -t $(LOGS_DIR)/build_*.log 2>/dev/null | head -1) 2>/dev/null || echo "No build logs found"

# View latest run log
log-run:
	@tail -50 $$(ls -t $(LOGS_DIR)/run_*.log 2>/dev/null | head -1) 2>/dev/null || echo "No run logs found"

# View all sessions
sessions:
	@echo "All sessions in $(SESSIONS_DIR):"
	@ls -lhtr $(SESSIONS_DIR)/ 2>/dev/null || echo "No sessions found yet"

# View latest session
latest-session:
	@SESSION=$$(ls -t $(SESSIONS_DIR)/ 2>/dev/null | head -1); \
	if [ -n "$$SESSION" ]; then \
		echo "Latest session: $(SESSIONS_DIR)/$$SESSION"; \
		ls -lh $(SESSIONS_DIR)/$$SESSION/; \
	else \
		echo "No sessions found yet"; \
	fi

# Verify library dependencies
verify:
	@echo "Checking library dependencies for $(TARGET)..."
	@if [ -f $(TARGET) ]; then \
		ldd $(TARGET) | grep -i stringutils && echo "✓ Shared library correctly linked" || echo "✗ Shared library not found"; \
	else \
		echo "✗ Executable not found. Please run 'make' first"; \
	fi

# Rebuild from scratch
rebuild: clean all

# Phony targets
.PHONY: all run clean rebuild logs log-build log-run sessions latest-session verify
