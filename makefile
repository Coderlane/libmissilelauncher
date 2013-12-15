# Generic Makefile
# Author: Travis Lane
# Feel free to use this file
# it can handle basic src,bin,obj based project structure

PROJECT=ThunderLauncher

CC=clang
CFLAGS=-c -Wall -std=gnu99 -Wextra

SRC_DIR=src
SOURCES=$(wildcard $(SRC_DIR)/*.c)

OBJ_DIR=obj
OBJECTS=$(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%_$(MODE).o,$(SOURCES))

TEST_DIR=test
TEST_BIN_DIR=bin/test
TEST_SOURCES=$(wildcard $(TEST_DIR)/*.c)
TEST_OBJECTS=$(patsubst $(TEST_DIR)/%.c,$(OBJ_DIR)/%_$(MODE).o,$(TEST_SOURCES))

BIN=$(PROJECT)
BIN_DIR=bin
TARGET=$(BIN_DIR)/$(BIN)

DEBUG_BIN=$(PROJECT)_Debug
DEBUG_TARGET=$(BIN_DIR)/$(DEBUG_BIN)

VALGRIND_BIN=$(PROJECT)_VALGRIND
VALGRIND_TARGET=$(BIN_DIR)/$(VALGRIND_BIN)

# All defaults to debug, to make release run make release
.PHONY: all dir debug release valgrind debug_build release_build valgrind_build clean
all: dir debug

release: dir release_build

debug: dir debug_build

valgrind: dir valgrind_build


# Check directories
dir:
	test -d $(BIN_DIR) || mkdir $(BIN_DIR)
	test -d $(TEST_BIN_DIR) || mkdir $(TEST_BIN_DIR)
	test -d $(OBJ_DIR) || mkdir $(OBJ_DIR)

# Clean up directories
clean:
	rm -f $(TARGET) $(DEBUG_TARGET) $(VALGRIND_TARGET) $(OBJ_DIR)/*.o 

# DEBUG target
debug_build: CFLAGS += -O0 -Wno-unused-variable -Wno-unused-parameter
debug_build: MODE=debug
debug_build: $(OBJECTS)
	$(CC) -o $(DEBUG_TARGET) $^
	

# Test targets
test_build: CFLAGS += -O0
test_build: MODE=test
test_build: $(OBJECTS) $(TEST_OBJECTS)
	$(CC) -o $(TEST_BIN_DIR)/movement $(OBJECTS) movement.o

# RELEASE target
release_build: CFLAGS += -O3 -DNDEBUG
release_build: MODE=release
release_build: $(OBJECTS)
	$(CC) -o $(TARGET) $^
 
# Make object files 
$(OBJ_DIR)/%_$(MODE).o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -o $@ $<

