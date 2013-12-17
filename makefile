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
RELEASE_OBJECTS=$(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%_release.o,$(SOURCES))
DEBUG_OBJECTS=$(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%_debug.o,$(SOURCES))
TEST_OBJECTS=$(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%_test.o,$(SOURCES))


TEST_DIR=test

BIN_DIR=bin

# All defaults to debug, to make release run make release
.PHONY: all dir debug release valgrind debug_build release_build valgrind_build clean test
all: test 

release: dir release_build

debug: dir debug_build

test: dir test_build


# Check directories
dir:
	test -d $(BIN_DIR) || mkdir $(BIN_DIR)
	test -d $(OBJ_DIR) || mkdir $(OBJ_DIR)

# Clean up directories
clean:
	rm -f $(BIN_DIR)/*_test $(OBJ_DIR)/*.o 

# DEBUG target
debug_build: CFLAGS += -O0 -Wno-unused-variable -Wno-unused-parameter
debug_build: MODE=debug
debug_build: $(DEBUG_OBJECTS)
# Change to ar
#	$(CC) -o $(DEBUG_TARGET) $^
	

# Test targets
test_build: CFLAGS += -O0  -Wno-unused-variable -Wno-unused-parameter
test_build: MODE=test
test_build: $(TEST_OBJECTS) detection_test


detection_test: MODE=test
detection_test: $(TEST_OBJECTS) $(OBJ_DIR)/detection_test.o
	$(CC) -o $(BIN_DIR)/detection_test $^

# RELEASE target
release_build: CFLAGS += -O3 -DNDEBUG
release_build: MODE=release
release_build: $(RELEASE_OBJECTS)
# Change to ar
# $(CC) -o $(TARGET) $^
 
# Make object files 
$(OBJ_DIR)/%_release.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -o $@ $<

# Make object files 
$(OBJ_DIR)/%_debug.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -o $@ $<

# Make object files 
$(OBJ_DIR)/%_test.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -o $@ $<

$(OBJ_DIR)/%_test.o: $(TEST_DIR)/%.c
	$(CC) $(CFLAGS) -o $@ $<
