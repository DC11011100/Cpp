#!/bin/bash
#TARGET    = pattern_match
TARGET = hackerRank
TEST_DICT = ./tests/test_dict.txt

# Compiler options
CXX      := g++
CXXFLAGS := -std=c++11 -Wall -g -Werror

# Libararies
INC_DIR := ./inc
HEADERS := ${wildcard ${INC_DIR}/*.h}

LFLAGS  := -I ${HEADERS}

# Source
SRC_DIR := ./src
SOURCES := ${wildcard ${SRC_DIR}/*.cpp}

${TARGET}: ${SOURCES}
	${CXX} ${CXXFLAGS} ${LFLAGS} -o $@ ${SOURCES}

.PHONY: version
version:
	git add ../aho-corasick
	git commit -m "new version"
	git push

.PHONY: leakcheck
leakcheck:
	valgrind --leak-check=yes ./${TARGET} ${TEST_DICT}

.PHONY: clean
clean:
	@rm -r ${TARGET} ./*.dSYM
