#include <iostream>
#include <fstream>
#include "lexer.hpp"

int main(int argc, char** argv) {
    std::string src("SELECT a FROM b");

    // 打开源文件
    std::ifstream src_file;
    src_file.open(argv[1]);
    if(!src_file.is_open()) {
        printf("[Error] Fail to read source file.\n");
        return -1;
    }
    char* buf = new char[1000];
    while(src_file >> buf) {
        src += buf;
    }
    printf("[Debug] source code: %s\n", src);
    // 构造词法分析器
    Lexer lexer(src);
    src_file.close();
}