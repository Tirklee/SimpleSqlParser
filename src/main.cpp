#include <iostream>
#include <fstream>
#include <string.h>
#include "lexer.hpp"

// 将关键字添加到符号表中
void add_keyword(Lexer* lexer) {
    const char* src = "SELECT FROM WHERE AS INSERT INTO VALUES \
                UPDATE DELETE JOIN LEFT RIGHT MIN MAX AVG SUM \
                UNION ALL GROUPBY HAVING DISTINCT ORDERBY TRUE FALSE \
                FALSE IS NOT NULL";
    lexer->run((char*)src);
}

int main(int argc, char** argv) {
    printf("Hello SQL Lexer!\n");

    Lexer lexer;
    // add_keyword(&lexer);
    const char* src = "12345 67 55.8 564";
    lexer.run((char*)src);
}