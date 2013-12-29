# Generic Makefile
# Author: Travis Lane
# Feel free to use this file
# it can handle basic src,bin,obj based project structure

PROJECT=thunderlauncher

LIBUSB_INCLUDE_PATH=/usr/include/libusb-1.0


CC=clang
CFLAGS=-c -Wall -fPIC -std=gnu99 -Wextra -I$(LIBUSB_INCLUDE_PATH)
PWD=$(shell pwd)

SRC_DIR=src
OBJ_DIR=obj
LIB_DIR=lib
TEST_DIR=test
BIN_DIR=bin

LIB_VERSION=1


SOURCES=$(wildcard $(SRC_DIR)/*.c)
TEST_SOURCES=$(wildcard $(TEST_DIR)/*.c)

TESTS=$(patsubst $(TEST_DIR)/%.c, %, $(TEST_SOURCES))

RELEASE_OBJECTS=$(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%_release.o,$(SOURCES))
DEBUG_OBJECTS=$(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%_debug.o,$(SOURCES))
TEST_OBJECTS=$(patsubst $(TEST_DIR)/%.c,$(OBJ_DIR)/%_test.o,$(TEST_SOURCES))

TEST_BINS=$(patsubst $(TEST_DIR)/%.c, $(BIN_DIR)/%_test, $(TEST_SOURCES))

LIBRARY_NAME=l$(PROJECT)
DEBUG_LIBRARY_NAME=l$(PROJECT)_debug

LIBRARY_PATH=$(PWD)/$(LIB_DIR)
INCLUDE_PATH=$(PWD)/$(SRC_DIR)

DEBUG_LIB=lib$(PROJECT)_debug.so
RELEASE_LIB=lib$(PROJECT).so

DEBUG_TARGET=$(LIB_DIR)/$(DEBUG_LIB)
RELEASE_TARGET=$(LIB_DIR)/$(RELEASE_LIB)



# All defaults to debug, to make release run make release
.PHONY: all dir debug release clean test debug_build release_build 
all: test 

release: dir release_build

debug: dir debug_build

test: dir debug_build test_build


#Check directories
dir:
	test -d $(BIN_DIR) || mkdir $(BIN_DIR)
	test -d $(OBJ_DIR) || mkdir $(OBJ_DIR)
	test -d $(LIB_DIR) || mkdir $(LIB_DIR)

# Clean up directories
clean:
	rm -f $(BIN_DIR)/*_test $(OBJ_DIR)/*.o $(LIB_DIR)/*.so

# Test targets
test_build: CFLAGS += -Wno-unused-variable -Wno-unused-parameter
test_build: $(DEBUG_TARGET) $(TEST_BINS)

# RELEASE target
release_build: CFLAGS += -DNDEBUG
release_build: $(RELEASE_OBJECTS)
	$(CC) -shared -Wl,-soname,$(RELEASE_LIB) -o $(RELEASE_TARGET) $^

$(DEBUG_TARGET): $(DEBUG_OBJECTS)
	$(CC) -shared -Wl,-soname,$(DEBUG_LIB) -o $(DEBUG_TARGET) $^

# Make object files 
$(OBJ_DIR)/%_release.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -o $@ $<

# Make object files 
$(OBJ_DIR)/%_debug.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -o $@ $<

# Make object files 
$(OBJ_DIR)/%_test.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -o $@ $< 

$(BIN_DIR)/%_test: $(DEBUG_LIBRARY) $(TEST_DIR)/%.c
	$(CC) -Wl,-rpath,$(LIBRARY_PATH) -L$(LIBRARY_PATH) -I$(INCLUDE_PATH) -o $@ $< -$(DEBUG_LIBRARY_NAME) -lusb-1.0

$(OBJ_DIR)/%_test.o: $(TEST_DIR)/%.c
	$(CC) $(CFLAGS) -o $@ $<
