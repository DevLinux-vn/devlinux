CC = gcc

CFLAGS = -Iinclude

PICFLAGS = -fPIC

OBJDIR = obj
LIBDIR = lib
BINDIR = bin

STATIC_LIB = $(LIBDIR)/libcalc.a
SHARED_LIB = $(LIBDIR)/liblogger.so

CALC_OBJ = $(OBJDIR)/calc.o
LOGGER_OBJ = $(OBJDIR)/logger.o

TARGET = $(BINDIR)/main

all: static shared $(TARGET)

static: $(STATIC_LIB)

shared: $(SHARED_LIB)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(LIBDIR):
	mkdir -p $(LIBDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

$(OBJDIR)/calc.o: src/calc.c include/calc.h | $(OBJDIR)
	$(CC) -c src/calc.c $(CFLAGS) -o $@

$(OBJDIR)/logger.o: src/logger.c include/logger.h | $(OBJDIR)
	$(CC) $(PICFLAGS) -c src/logger.c $(CFLAGS) -o $@

$(STATIC_LIB): $(CALC_OBJ) | $(LIBDIR)
	ar rcs $@ $^

$(SHARED_LIB): $(LOGGER_OBJ) | $(LIBDIR)
	$(CC) -shared -o $@ $^

$(TARGET): app/main.c $(STATIC_LIB) $(SHARED_LIB) | $(BINDIR)
	$(CC) app/main.c $(CFLAGS) \
	-L$(LIBDIR) -lcalc -llogger \
	-Wl,-rpath,'$$ORIGIN/../lib' \
	-lm \
	-o $@

run: all
	./$(TARGET)

clean:
	rm -rf $(OBJDIR) $(LIBDIR) $(BINDIR) app.log

rebuild: clean all
