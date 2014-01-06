# Generic Makefile
# Author: Travis Lane
# Feel free to use this file
# it can handle basic src,bin,obj based project structure

PROJECT=thunderlauncher

LIBUSB_INCLUDE_PATH=/usr/include/libusb-1.0


CC=clang
CFLAGS=-c -Wall -fPIC -std=gnu99 -Wextra -D$(SYSTEM) -I$(LIBUSB_INCLUDE_PATH) 

PWD=$(shell pwd)
UNAME=$(shell uname)

# Detect system
ifeq ($(UNAME), Windows)
	SYSTEM=WINDOWS
	DEBUG_LIB=lib$(PROJECT)_debug.dll
	RELEASE_LIB=lib$(PROJECT).dll
else 
ifeq ($(UNAME), Darwin)
	SYSTEM=DARWIN
	DEBUG_LIB=lib$(PROJECT)_debug.so
	RELEASE_LIB=lib$(PROJECT).so
else
ifeq ($(UNAME), Linux)
	SYSTEM=LINUX
	DEBUG_LIB=lib$(PROJECT)_debug.so
	RELEASE_LIB=lib$(PROJECT).so
else
	# Add other systems here
	SYSTEM=UNKNOWN
endif
endif
endif

SRC_DIR=src
OBJ_DIR=obj
LIB_DIR=lib
TEST_DIR=test
BIN_DIR=bin
EXAMPLE_DIR=examples

TEST_BIN_DIR=$(TEST_DIR)/$(BIN_DIR)
EXAMPLE_BIN_DIR=$(EXAMPLE_DIR)/$(BIN_DIR)

LIB_VERSION=1

SOURCES=$(wildcard $(SRC_DIR)/*.c)
TEST_SOURCES=$(wildcard $(TEST_DIR)/*.c)
EXAMPLE_SOURCES=$(wildcard $(EXAMPLE_DIR)/*.c)

RELEASE_OBJECTS=$(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%_release.o,$(SOURCES))
DEBUG_OBJECTS=$(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%_debug.o,$(SOURCES))
TEST_OBJECTS=$(patsubst $(TEST_DIR)/%.c,$(OBJ_DIR)/%_test.o,$(TEST_SOURCES))

TEST_BINS=$(patsubst $(TEST_DIR)/%.c, $(TEST_BIN_DIR)/%_test, $(TEST_SOURCES))
EXAMPLE_BINS=$(patsubst $(EXAMPLE_DIR)/%.c, $(EXAMPLE_BIN_DIR)/%_example, $(EXAMPLE_SOURCES))

LIBRARY_NAME=l$(PROJECT)
DEBUG_LIBRARY_NAME=l$(PROJECT)_debug

LIBRARY_PATH=$(PWD)/$(LIB_DIR)
INCLUDE_PATH=$(PWD)/$(SRC_DIR)

DEBUG_TARGET=$(LIB_DIR)/$(DEBUG_LIB)
RELEASE_TARGET=$(LIB_DIR)/$(RELEASE_LIB)

.PHONY: all clean dir debug example example_dir install release test test_dir  
all: release 

release: dir $(RELEASE_TARGET) 

debug: dir $(DEBUG_TARGET) 

test: test_dir test_build

example: example_dir example_build

#Check directories
dir:
	test -d $(OBJ_DIR) || mkdir $(OBJ_DIR)
	test -d $(LIB_DIR) || mkdir $(LIB_DIR)

test_dir: dir
	test -d $(TEST_BIN_DIR) || mkdir $(TEST_BIN_DIR)

example_dir: dir
	test -d $(EXAMPLE_BIN_DIR) || mkdir $(EXAMPLE_BIN_DIR)

# Clean up directories
clean:
	rm -f $(OBJ_DIR)/*.o $(LIB_DIR)/*.so $(LIB_DIR)/*.dll $(TEST_BIN_DIR)/*_test $(EXAMPLE_BIN_DIR)/*_example

# Release target
$(RELEASE_TARGET): CFLAGS += -DNDEBUG 
$(RELEASE_TARGET): $(RELEASE_OBJECTS)
	$(CC) -shared -Wl,-soname,$(RELEASE_LIB) -o $(RELEASE_TARGET) $^

#d Debug Target
$(DEBUG_TARGET): $(DEBUG_OBJECTS)
	$(CC) -shared -Wl,-soname,$(DEBUG_LIB) -o $(DEBUG_TARGET) $^

# Test targets
test_build: $(DEBUG_TARGET) $(TEST_BINS)

# Example targets
example_build: $(RELEASE_TARGET) $(EXAMPLE_BINS)

install:
	echo "Not yet implemented. The libraries you want are in lib for now"

# Make release object files 
$(OBJ_DIR)/%_release.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -o $@ $<

# Make debug object files 
$(OBJ_DIR)/%_debug.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -o $@ $<

# Make test object files
$(OBJ_DIR)/%_test.o: $(TEST_DIR)/%.c
	$(CC) $(CFLAGS) -o $@ $<

# Create test binaries
$(TEST_BIN_DIR)/%_test: $(DEBUG_LIBRARY) $(TEST_DIR)/%.c
	$(CC) -Wl,-rpath,$(LIBRARY_PATH) -L$(LIBRARY_PATH) -I$(INCLUDE_PATH) -o $@ $< -$(DEBUG_LIBRARY_NAME) -lusb-1.0

# Create example binaries
$(EXAMPLE_BIN_DIR)/%_example: $(DEBUG_LIBRARY) $(EXAMPLE_DIR)/%.c
	$(CC) -Wl,-rpath,$(LIBRARY_PATH) -L$(LIBRARY_PATH) -I$(INCLUDE_PATH) -o $@ $< -$(DEBUG_LIBRARY_NAME) -lusb-1.0