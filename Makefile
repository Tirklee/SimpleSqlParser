SRC_DIR	    := src
INCLUDE_DIR	:= include
BUILD_DIR	:= build

SRCS      	:= $(shell find $(SRC_DIR) -name *.cpp)
OBJS      	:= $(BUILD_DIR)/lexer.cpp.o \
			   $(BUILD_DIR)/main.cpp.o

CXX			:= g++
CXXFLAGS  	:= -g -Wall -O3 -std=c++17 -MMD

.PHONY: build run clean

$(BUILD_DIR)/%.cpp.o: $(SRC_DIR)/%.cpp
	@$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -c -o $@ $<

run: $(OBJS)
	@echo "run"
