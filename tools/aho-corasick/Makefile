#!/bin/bash
TARGET = pattern_match

# Compiler options
CXX      := g++
CXXFLAGS := -Wall

# Libararies
INC_DIR := ./inc
HEADERS := ${wildcard ${INC_DIR}/*.h}

LFLAGS  := -I ${HEADERS}

# Source
SRC_DIR := ./src
SOURCES := ${wildcard ${SRC_DIR}/*.cpp}

${TARGET}: ${SOURCES}
	${CXX} ${LFLAGS} -o $@ ${SOURCES}

.PHONY: clean
clean:
	@rm ${TARGET}


.PHONY: version
version:
	git add ./Makefile ${HEADERS} ${SOURCES}
	git commit -m"new version"
	git push
