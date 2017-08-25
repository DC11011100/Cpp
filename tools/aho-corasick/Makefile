#!/bin/bash
TARGET = pattern_match

# Compiler options
CXX      = g++
CXXFLAGS := -Wall

# Source
SRC_DIR := ./src

all: ${SRC_DIR}/${TARGET}.cpp
	${CXX} -o ${TARGET} ${SRC_DIR}/${TARGET}.cpp

.PHONY: clean
clean:
	@rm ${TARGET}

