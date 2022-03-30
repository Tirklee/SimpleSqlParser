#include <iostream>
#include <fstream>
#include <string.h>
#include "lexer.hpp"

// 将关键字添加到符号表中
void add_keyword(Lexer* lexer) {
    char* src = "SELECT FROM WHERE AS INSERT INTO VALUES \ 
                UPDATE DELETE JOIN LEFT RIGHT MIN MAX AVG SUM \
                UNION ALL GROUPBY HAVING DISTINCT ORDERBY TRUE FALSE \
                FALSE IS NOT NULL";
    lexer->run(src);
}

int main(int argc, char** argv) {
    char* src = new char[1000];
    printf("Hello SQL Lexer!\n");

    Lexer lexer;
    add_keyword(&lexer);

    // // 打开源文件
    // std::ifstream src_file;
    // src_file.open(argv[1]);
    // if(!src_file.is_open()) {
    //     printf("[Error] Fail to read source file.\n");
    //     return -1;
    // }
    // char* buf = new char[100];
    // while(src_file >> buf) {
    //     strcat(src, buf);
    // }
    // printf("[Debug] source code: %s\n", src);
    // // 构造词法分析器
    // Lexer lexer(src);
    // src_file.close();
}