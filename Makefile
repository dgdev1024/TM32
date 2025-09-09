# TM32 Virtual CPU Emulator Makefile

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinclude
SRCDIR = src
INCDIR = include
BUILDDIR = build
TARGET = tm32

# Source files
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)

# Default target
all: $(TARGET)

# Create build directory
$(BUILDDIR):
	mkdir -p $(BUILDDIR)

# Compile object files
$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Link the final executable
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

# Clean build artifacts
clean:
	rm -rf $(BUILDDIR) $(TARGET)

# Run tests
test: $(TARGET)
	./$(TARGET) test

# Install (copy to /usr/local/bin)
install: $(TARGET)
	sudo cp $(TARGET) /usr/local/bin/

# Phony targets
.PHONY: all clean test install

# Show help
help:
	@echo "TM32 Virtual CPU Emulator Build System"
	@echo ""
	@echo "Targets:"
	@echo "  all      - Build the TM32 emulator (default)"
	@echo "  clean    - Remove build artifacts"
	@echo "  test     - Build and run tests"
	@echo "  install  - Install to /usr/local/bin"
	@echo "  help     - Show this help message"
	@echo ""
	@echo "Usage examples:"
	@echo "  make            # Build the project"
	@echo "  make test       # Build and test"
	@echo "  make clean      # Clean build files"