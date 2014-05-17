# Makefile for libmissilelauncher examples
# Author: Travis Lane
# Version: 0.1.0
#
# Options:
# make (default) 	Build the examples
# make clean			Clean up all files

CC=clang
CFLAGS=-Wall -std=gnu99 -Wextra 

PWD=$(shell pwd)
 
SRC_DIR=.
OBJ_DIR=obj
BIN_DIR=bin

SOURCES=$(wildcard $(SRC_DIR)/*.c)
OBJECTS=$(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))
EXAMPLES=$(patsubst $(SRC_DIR)/%.c,%_example,$(SOURCES))
TARGETS=$(patsubst $(SRC_DIR)/%.c,$(BIN_DIR)/%_example,$(SOURCES))

.PHONY: all dir clean build

all: dir build

# Check directories
dir:
	test -d $(OBJ_DIR) || mkdir $(OBJ_DIR)
	test -d $(BIN_DIR) || mkdir $(BIN_DIR)

# Clean up
clean: 
	rm -f $(OBJ_DIR)/*.o
	rm -f $(BIN_DIR)/*_example

# Main Build task
build: $(OBJECTS) $(TARGETS)

# Make objects
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c $(CFLAGS) -o $@ $<

# Make executables
$(BIN_DIR)/%_example: $(OBJ_DIR)/%.o
	$(CC) $(CFLAGS) -o $@ $< -lmissilelauncher -lusb-1.0
