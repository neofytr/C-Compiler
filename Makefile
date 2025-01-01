# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra

# Directories
SRC_DIR = src
TEST_DIR = neocc/testing
PROD_DIR = neocc/production

# Source and output
SRCS = $(SRC_DIR)/compiler.c
TEST_TARGET = $(TEST_DIR)/neocc_test
PROD_TARGET = $(PROD_DIR)/neocc_prod

# Common flags for both targets
COMMON_FLAGS = $(CFLAGS)

# Debug-specific flags
DEBUG_FLAGS = -O0 -g3 -DDEBUG

# Production-specific flags
PROD_FLAGS = -O3 -march=native -flto -DNDEBUG

# Build rules
all: test production

# Create directories if they don't exist
$(TEST_DIR) $(PROD_DIR):
	mkdir -p $@

# Build the testing binary
test: $(TEST_TARGET)

$(TEST_TARGET): $(SRCS) | $(TEST_DIR)
	$(CC) $(COMMON_FLAGS) $(DEBUG_FLAGS) -o $@ $<

# Build the production binary
production: $(PROD_TARGET)

$(PROD_TARGET): $(SRCS) | $(PROD_DIR)
	$(CC) $(COMMON_FLAGS) $(PROD_FLAGS) -o $@ $<

# Clean build artifacts
clean:
	rm -rf $(TEST_DIR) $(PROD_DIR)

# Phony targets
.PHONY: all test production clean