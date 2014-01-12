# Makefile for libmissilelauncher tests
# Author: Travis Lane
# Version: 0.1.0
#
# Options:
# make (default) 	Build the tests 
# make clean			Clean up all files

PROJECT=missilelauncher

CC=clang
CFLAGS=-Wall -std=gnu99 -Wextra

PWD=$(shell pwd)

SRC_DIR=.
OBJ_DIR=obj
BIN_DIR=bin

SOURCES=$(wildcard $(SRC_DIR)/*.c)
OBJECTS=$(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))
EXAMPLES=$(patsubst $(SRC_DIR)/%.c,%,$(SOURCES))
TARGETS=$(patsubst $(SRC_DIR)/%.c,$(BIN_DIR)/%_test,$(SOURCES))

LIBRARY_NAME=l$(PROJECT)_debug
LIBRARY_PATH=$(PWD)/../lib
INCLUDE_PATH=$(PWD)/../src

.PHONY: all dir clean build

all: dir build

# Check directories
dir:
	test -d $(OBJ_DIR) || mkdir $(OBJ_DIR)
	test -d $(BIN_DIR) || mkdir $(BIN_DIR)

# Clean up
clean: 
	rm -f $(OBJ_DIR)/*.o
	rm -f $(BIN_DIR)/*_test

# Main Build task
build: library $(OBJECTS) $(TARGETS)

# Make sure that the library has been built
library: 
	cd .. && $(MAKE) debug 

# Make objects
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c $(CFLAGS) -o $@ $<

# Make executables
$(BIN_DIR)/%_test: $(OBJ_DIR)/%.o
	$(CC) $(CFLAGS) -Wl,-rpath,$(LIBRARY_PATH) -L$(LIBRARY_PATH) -I$(INCLUDE_PATH) -o $@ $< -$(LIBRARY_NAME) -lusb-1.0
