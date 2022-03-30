#include <iostream>
#include <fstream>
#include <string.h>
#include "lexer.hpp"


int main(int argc, char** argv) {
    printf("Hello SQL Lexer!\n");

    Lexer lexer;
    // 将关键字加入到符号表中
    lexer.add_keywords();
    const char* src = "12345 67 55.8 564";
    lexer.run((char*)src);
}