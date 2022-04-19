#include <iostream>
#include <fstream>
#include <string.h>
#include "lexer.hpp"


int main(int argc, char** argv) {
    printf("Hello SQL Lexer!\n");
    Lexer lexer;
    // 对词法分析器进行初始化
    lexer.init();
    // printf("[Debug] Test Number.\n");
    // const char* src = "12345 67 55.8 564";
    // lexer.run((char*)src);
    // printf("[Debug] Test Operator.\n");
    // src = "= > >= < <= <=> != && || . AND OR XOR";
    // lexer.run((char*)src);
    // printf("[Debug] Test String.\n");
    // src = "\"Hello SQL Lexer!\"";
    // lexer.run((char*)src);
    // src = "SELECT t.c FROM t WHERE t.a > 0 GROUP BY ORDER BY ON DEFAULT NOT !";
    // lexer.run((char*)src);

    FILE* file = fopen("examples/test1.txt", "r");
    if(file){
        char* buf = new char[512];
        fread(buf, 1, 512, file);
        lexer.run(buf);
    }
}