SRC_DIR	    := src
INCLUDE_DIR	:= include
BUILD_DIR	:= build

SRCS      	:= $(shell find $(SRC_DIR) -name *.cpp)
OBJS      	:= $(BUILD_DIR)/lexer.o \
			   $(BUILD_DIR)/dfa.o \
			   $(BUILD_DIR)/main.o \
			#    $(BUILD_DIR)/utils.o \
			#    $(BUILD_DIR)/GrammaAnalyse.o 

CXX			:= g++
CXXFLAGS  	:= -g -Wall -Werror -std=c++17 -D LEXER

.PHONY: run clean

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -c $(SRC_DIR)/lexer.cpp -o $(BUILD_DIR)/lexer.o 
	@$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -c $(SRC_DIR)/dfa.cpp -o $(BUILD_DIR)/dfa.o 
	@$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -c $(SRC_DIR)/main.cpp -o $(BUILD_DIR)/main.o
	@$(CXX) $(OBJS) -o $(BUILD_DIR)/main

run: $(OBJS)
	@./build/main examples/test0B.txt


clean:
	@rm $(BUILD_DIR)/*.o $(BUILD_DIR)/main
