CUR_DIR := $(shell pwd)
INCLUDE_DIR := $(CUR_DIR)/include
SRC_DIR := $(CUR_DIR)/src
OBJ_DIR := $(CUR_DIR)/obj
LIB_DIR := $(CUR_DIR)/lib
BIN_DIR := $(CUR_DIR)/bin
LOGS_DIR := $(CUR_DIR)/logs
SESSIONS_DIR := $(LOGS_DIR)/sessions

# Compiler and tools
CC := gcc
AR := ar

# Compiler flags
CFLAGS := -Wall -Wextra -I$(INCLUDE_DIR)
FPIC_FLAGS := -fPIC
MATH_FLAGS := -lm

# Library names
STATIC_LIB := $(LIB_DIR)/libcalc.a
SHARED_LIB := $(LIB_DIR)/liblogger.so
TARGET := $(BIN_DIR)/app

# Source and object files
CALC_OBJ := $(OBJ_DIR)/calc.o
LOGGER_OBJ := $(OBJ_DIR)/logger.o
MAIN_OBJ := $(OBJ_DIR)/main.o

# Generate timestamp for logging
TIMESTAMP := $(shell date +"%Y%m%d_%H%M%S")
BUILD_LOG := $(LOGS_DIR)/build_$(TIMESTAMP).log
RUN_LOG := $(LOGS_DIR)/run_$(TIMESTAMP).log
SESSION_DIR := $(SESSIONS_DIR)/session_$(TIMESTAMP)

# Ensure directories exist
_unused := $(shell mkdir -p $(OBJ_DIR) $(LIB_DIR) $(BIN_DIR) $(LOGS_DIR) $(SESSIONS_DIR))

# Default target
all: $(TARGET)
	@echo "✓ Build completed successfully at $(shell date '+%Y-%m-%d %H:%M:%S')" | tee -a $(BUILD_LOG)

# Build both libraries
libs: static shared

# Build static library
static: $(STATIC_LIB)

$(STATIC_LIB): $(CALC_OBJ)
	@echo "[BUILD] Creating static library..." | tee -a $(BUILD_LOG)
	$(AR) rcs $@ $^ 2>&1 | tee -a $(BUILD_LOG)
	@echo "  ✓ Static library created: $(STATIC_LIB)" | tee -a $(BUILD_LOG)

# Build shared library
shared: $(SHARED_LIB)

$(SHARED_LIB): $(LOGGER_OBJ)
	@echo "[BUILD] Creating shared library..." | tee -a $(BUILD_LOG)
	$(CC) -shared -o $@ $^ $(MATH_FLAGS) 2>&1 | tee -a $(BUILD_LOG)
	@echo "  ✓ Shared library created: $(SHARED_LIB)" | tee -a $(BUILD_LOG)

# Compile calc.c to object file
$(CALC_OBJ): $(SRC_DIR)/calc.c
	@echo "[BUILD] Compiling: $<" | tee -a $(BUILD_LOG)
	$(CC) $(CFLAGS) -c $< -o $@ 2>&1 | tee -a $(BUILD_LOG)

# Compile logger.c with PIC for shared library
$(LOGGER_OBJ): $(SRC_DIR)/logger.c
	@echo "[BUILD] Compiling (PIC): $<" | tee -a $(BUILD_LOG)
	$(CC) $(CFLAGS) $(FPIC_FLAGS) -c $< -o $@ 2>&1 | tee -a $(BUILD_LOG)

# Compile main.c
$(MAIN_OBJ): $(SRC_DIR)/main.c
	@echo "[BUILD] Compiling: $<" | tee -a $(BUILD_LOG)
	$(CC) $(CFLAGS) -c $< -o $@ 2>&1 | tee -a $(BUILD_LOG)

# Link executable with both libraries
$(TARGET): $(MAIN_OBJ) $(STATIC_LIB) $(SHARED_LIB)
	@echo "[BUILD] Linking: $@" | tee -a $(BUILD_LOG)
	$(CC) $(CFLAGS) -o $@ $(MAIN_OBJ) -L$(LIB_DIR) -lcalc -llogger $(MATH_FLAGS) 2>&1 | tee -a $(BUILD_LOG)
	@echo "  ✓ Executable created: $(TARGET)" | tee -a $(BUILD_LOG)
	@echo "[BUILD] Verifying library dependencies..." | tee -a $(BUILD_LOG)
	@LD_LIBRARY_PATH=$(LIB_DIR):$$LD_LIBRARY_PATH ldd $(TARGET) 2>&1 | tee -a $(BUILD_LOG)

# Run the application
run: $(TARGET)
	@mkdir -p $(SESSION_DIR)
	@echo "" | tee -a $(RUN_LOG)
	@echo "========================================" | tee -a $(RUN_LOG)
	@echo "Run started at $(shell date '+%Y-%m-%d %H:%M:%S')" | tee -a $(RUN_LOG)
	@echo "========================================" | tee -a $(RUN_LOG)
	@echo "Running: $(TARGET)" | tee -a $(RUN_LOG)
	@echo "========================================" | tee -a $(RUN_LOG)
	@LD_LIBRARY_PATH=$(LIB_DIR):$$LD_LIBRARY_PATH $(TARGET) 2>&1 | tee -a $(RUN_LOG)
	@echo "========================================" | tee -a $(RUN_LOG)
	@echo "Run completed at $(shell date '+%Y-%m-%d %H:%M:%S')" | tee -a $(RUN_LOG)
	@echo "========================================" | tee -a $(RUN_LOG)
	@echo ""
	@echo "[SESSION] Consolidating logs..."
	@cp $(LOGS_DIR)/build_$(TIMESTAMP).log $(SESSION_DIR)/ 2>/dev/null || true
	@cp $(LOGS_DIR)/run_$(TIMESTAMP).log $(SESSION_DIR)/ 2>/dev/null || true
	@echo "[SESSION] All logs saved to session folder"
	@echo "✓ Session path: $(SESSION_DIR)"
	@echo ""
	@if [ -f app.log ]; then \
		echo "✓ Log file created: app.log"; \
		echo ""; \
		echo "--- app.log contents ---"; \
		cat app.log; \
		echo "--- end of app.log ---"; \
	fi

# Clean build artifacts
clean:
	rm -rf $(OBJ_DIR) $(LIB_DIR) $(BIN_DIR)
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

# Verify library linkage
verify:
	@echo "Checking library dependencies..."
	@LD_LIBRARY_PATH=$(LIB_DIR):$$LD_LIBRARY_PATH ldd $(TARGET) 2>/dev/null | grep -E "libcalc|liblogger" || echo "Libraries not found"
	@echo ""
	@echo "Static library symbols:"
	@nm $(STATIC_LIB) 2>/dev/null | grep -E "add|sub|mul|div" || echo "No symbols found"

# Rebuild from scratch
rebuild: clean all

# Phony targets
.PHONY: all libs static shared run clean rebuild logs log-build log-run sessions verify
