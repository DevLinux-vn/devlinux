CUR_DIR := $(shell pwd)
INCLUDE_DIR := $(CUR_DIR)/include
SRC_DIR := $(CUR_DIR)/src
APP_DIR := $(CUR_DIR)/app
OBJ_DIR := $(CUR_DIR)/obj
BIN_DIR := $(CUR_DIR)/bin
LIB_DIR := $(CUR_DIR)/lib
LOGS_DIR := $(CUR_DIR)/logs
SESSIONS_DIR := $(CUR_DIR)/logs/sessions

# Compiler and tools
CC := gcc
AR := ar

# Compiler flags
CFLAGS := -Wall -Wextra -I$(INCLUDE_DIR)

# Library and executable files
LIBNAME := libmathutils.a
STATIC_LIB := $(LIB_DIR)/$(LIBNAME)
TARGET := $(BIN_DIR)/app

# Source and object files
LIB_OBJS := $(OBJ_DIR)/mathutils.o
APP_OBJS := $(OBJ_DIR)/main.o

# Generate timestamp for logging
TIMESTAMP := $(shell date +"%Y%m%d_%H%M%S")
BUILD_LOG := $(LOGS_DIR)/build_$(TIMESTAMP).log
RUN_LOG := $(LOGS_DIR)/run_$(TIMESTAMP).log
SESSION_DIR := $(SESSIONS_DIR)/session_$(TIMESTAMP)

# Ensure directories exist
_unused := $(shell mkdir -p $(OBJ_DIR) $(BIN_DIR) $(LIB_DIR) $(LOGS_DIR) $(SESSIONS_DIR))

# Default target
all: $(TARGET)
	@echo "✓ Build completed successfully at $(shell date '+%Y-%m-%d %H:%M:%S')" | tee -a $(BUILD_LOG)

# Create static library
$(STATIC_LIB): $(LIB_OBJS)
	@echo "[BUILD] Creating static library..." | tee -a $(BUILD_LOG)
	$(AR) rcs $@ $^ 2>&1 | tee -a $(BUILD_LOG)
	@echo "  ✓ Static library created: $(STATIC_LIB)" | tee -a $(BUILD_LOG)

# Compile object files from src/
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "[BUILD] Compiling: $<" | tee -a $(BUILD_LOG)
	$(CC) $(CFLAGS) -c $< -o $@ 2>&1 | tee -a $(BUILD_LOG)

# Compile object files from app/
$(OBJ_DIR)/main.o: $(APP_DIR)/main.c
	@echo "[BUILD] Compiling: $<" | tee -a $(BUILD_LOG)
	$(CC) $(CFLAGS) -c $< -o $@ 2>&1 | tee -a $(BUILD_LOG)

# Link executable with static library
$(TARGET): $(APP_OBJS) $(STATIC_LIB)
	@echo "[BUILD] Linking: $@" | tee -a $(BUILD_LOG)
	$(CC) $(CFLAGS) -o $@ $(APP_OBJS) $(STATIC_LIB) 2>&1 | tee -a $(BUILD_LOG)
	@echo "  ✓ Executable created: $(TARGET)" | tee -a $(BUILD_LOG)

# Run the application
run: $(TARGET)
	@mkdir -p $(SESSION_DIR)
	@echo "" | tee -a $(RUN_LOG)
	@echo "========================================" | tee -a $(RUN_LOG)
	@echo "Run started at $(shell date '+%Y-%m-%d %H:%M:%S')" | tee -a $(RUN_LOG)
	@echo "========================================" | tee -a $(RUN_LOG)
	@echo "Running: $(TARGET)" | tee -a $(RUN_LOG)
	@echo "========================================" | tee -a $(RUN_LOG)
	@$(TARGET) 2>&1 | tee -a $(RUN_LOG)
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
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(LIB_DIR)
	@echo "[CLEAN] Build artifacts removed at $(shell date '+%Y-%m-%d %H:%M:%S')" | tee -a $(LOGS_DIR)/clean_$(TIMESTAMP).log

# View build logs
logs:
	@echo "Available logs in $(LOGS_DIR):"
	@ls -lhtr $(LOGS_DIR)/ 2>/dev/null || echo "No logs found yet"

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

# Rebuild from scratch
rebuild: clean all

# Phony targets
.PHONY: all run clean rebuild logs log-build log-run sessions latest-session
