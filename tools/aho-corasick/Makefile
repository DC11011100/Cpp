#!/bin/bash
TARGET = pattern_match

# Compiler options
CXX      := g++
CXXFLAGS := -std=c++11 -Wall

# Libararies
INC_DIR := ./inc
HEADERS := ${wildcard ${INC_DIR}/*.h}

LFLAGS  := -I ${HEADERS}

# Source
SRC_DIR := ./src
SOURCES := ${wildcard ${SRC_DIR}/*.cpp}

${TARGET}: ${SOURCES}
	${CXX} ${CXXFLAGS} ${LFLAGS} -o $@ ${SOURCES}

.PHONY: clean
clean:
	@rm ${TARGET}


.PHONY: version
version:
	git add ../aho-corasick
	git commit -m"new version : Aho-Corasick"
	git push
