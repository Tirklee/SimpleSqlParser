SRC_DIR	    := src
INCLUDE_DIR	:= include
BUILD_DIR	:= build

SRCS      	:= $(shell find $(SRC_DIR) -name *.cpp)
OBJS      	:= $(BUILD_DIR)/lexer.o \
			   $(BUILD_DIR)/main.o

CXX			:= g++
CXXFLAGS  	:= -g -Wall -std=c++17

.PHONY: build run clean

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -c $(SRC_DIR)/lexer.cpp -o $(BUILD_DIR)/lexer.o 
	@$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -c $(SRC_DIR)/main.cpp -o $(BUILD_DIR)/main.o 
	@$(CXX) $(OBJS) -o $(BUILD_DIR)/main

run: $(OBJS)
	@./build/main

clean:
	@rm build/*.o
