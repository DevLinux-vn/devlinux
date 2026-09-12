# Makefile - Exercise_3: static libcalc.a + shared liblogger.so
# Targets: all, libs, static, shared, run, clean, rebuild

CC       := gcc
CFLAGS   := -Wall -Wextra -Iinclude

OBJDIR   := obj
LIBDIR   := lib
BINDIR   := bin

STATIC_LIB := $(LIBDIR)/libcalc.a
SHARED_LIB := $(LIBDIR)/liblogger.so
EXE        := $(BINDIR)/app

# Default: build everything (libraries + executable)
all: $(EXE)

# Convenience target to build both libraries
libs: static shared

# ---- Static library: libcalc.a ----
static: $(STATIC_LIB)

$(OBJDIR)/calc.o: src/calc.c include/calc.h | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(STATIC_LIB): $(OBJDIR)/calc.o | $(LIBDIR)
	ar rcs $@ $^

# ---- Shared library: liblogger.so (compiled with -fPIC) ----
shared: $(SHARED_LIB)

$(OBJDIR)/logger.o: src/logger.c include/logger.h | $(OBJDIR)
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

$(SHARED_LIB): $(OBJDIR)/logger.o | $(LIBDIR)
	$(CC) -shared -o $@ $^

# ---- Main executable: links calc (static) and logger (shared) ----
$(EXE): src/main.c $(STATIC_LIB) $(SHARED_LIB) | $(BINDIR)
	$(CC) $(CFLAGS) src/main.c -L$(LIBDIR) -lcalc -llogger -o $(EXE) -lm

# Create directories if missing
$(OBJDIR):
	mkdir -p $(OBJDIR)
$(LIBDIR):
	mkdir -p $(LIBDIR)
$(BINDIR):
	mkdir -p $(BINDIR)

# Run: loader needs to find liblogger.so at runtime
run: $(EXE)
	LD_LIBRARY_PATH=$(LIBDIR) ./$(EXE)

# Remove all generated artifacts (obj/, lib/, bin/) and the log
clean:
	rm -rf $(OBJDIR) $(LIBDIR) $(BINDIR) app.log

# Clean then full rebuild
rebuild: clean all

.PHONY: all libs static shared run clean rebuild
