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

# Build rules
all: test production

# Ensure the testing directory exists
$(TEST_DIR):
	mkdir -p $(TEST_DIR)

# Ensure the production directory exists
$(PROD_DIR):
	mkdir -p $(PROD_DIR)

# Build the testing binary
test: $(TEST_TARGET)

$(TEST_TARGET): $(SRCS) | $(TEST_DIR)
	$(CC) $(CFLAGS) -O0 -g3 -o $@ $<

# Build the production binary
production: $(PROD_TARGET)

$(PROD_TARGET): $(SRCS) | $(PROD_DIR)
	$(CC) $(CFLAGS) -O3 -march=native -flto -o $@ $<

# Clean build artifacts
clean:
	rm -f $(TEST_TARGET) $(PROD_TARGET)

# Phony targets
.PHONY: all test production clean
