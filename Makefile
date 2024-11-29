# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O0 -g3 -I./src

# Directories
SRC_DIR = src
BIN_DIR = bin

# Output binary
TARGET = $(BIN_DIR)/neocc

# Source files
SRCS = $(SRC_DIR)/compiler.c

# Object files (place in src directory)
OBJS = $(SRCS:.c=.o)

# Default target
all: $(TARGET)

# Create binary directory if it doesn't exist
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Build the binary
$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(OBJS)
	rm $(SRC_DIR)/compiler.o

# Compile source files to object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets
.PHONY: all clean
