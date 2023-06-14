CC = gcc
CFLAGS = --std=c11 -D_DEFAULT_SOURCE -fsanitize=address -pedantic -pedantic-errors -Wall -Wextra -Wno-unused-parameter -Wno-implicit-fallthrough -D_POSIX_C_SOURCE=200112L -pthread -g
SRC_DIR = src
PATCH_DIR = patches
INCLUDE_DIR = $(SRC_DIR)/include
UTILS_DIR = $(SRC_DIR)/utils
PARSERS_DIR = $(SRC_DIR)/parsers
# TESTS_DIR = $(SRC_DIR)/tests
OBJ_DIR = obj
BIN_DIR = bin

# List of source files
SRCS = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(UTILS_DIR)/*.c) $(wildcard $(PARSERS_DIR)/*.c) $(wildcard $(TESTS_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

# Target executable
TARGET = run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(OBJ_DIR)/%.o: $(UTILS_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(OBJ_DIR)/%.o: $(PARSERS_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# $(OBJ_DIR)/%.o: $(TESTS_DIR)/%.c
# 	@mkdir -p $(@D)
# 	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

.PHONY: clean

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: patch

patch:
	for file in $(wildcard $(PATCH_DIR)/*.patch); do \
		patch -p1 < $$file; \
	done