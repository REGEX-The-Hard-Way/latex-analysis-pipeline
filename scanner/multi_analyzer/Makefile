# Makefile for Ragel-based multi_analyzer pipeline
# This builds the chainable text processing pipeline

CC = gcc
CFLAGS = -O2 -g -Wall -Wextra -std=c99 -D_GNU_SOURCE
LDFLAGS =

# Source files
RL_SOURCES = src/multi_analyzer.rl
C_SOURCES = src/main.c src/regex_util.c src/porter2.c

# Generated files (from .rl)
GENERATED = src/multi_analyzer.c

# Object files
OBJECTS = $(patsubst %.c,%.o,$(GENERATED) $(C_SOURCES))

TARGET = multi_analyzer.out

# Check if ragel is available
RAGEL ?= ragel

all: $(TARGET)

# Rule to compile .rl to .c
src/multi_analyzer.c: src/multi_analyzer.rl
	$(RAGEL) -G2 -o $@ $<

# Compile C files
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Link
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ -lm

clean:
	rm -f $(TARGET) $(OBJECTS) $(GENERATED) src/multi_analyzer.c

.PHONY: all clean

# Install
PREFIX ?= /usr/local
bindir ?= $(PREFIX)/bin

install: $(TARGET)
	mkdir -p $(DESTDIR)$(bindir)
	cp $(TARGET) $(DESTDIR)$(bindir)/multi_analyzer

uninstall:
	rm -f $(DESTDIR)$(bindir)/multi_analyzer

.PHONY: clean install uninstall
