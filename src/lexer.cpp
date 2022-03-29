#include "lexer.hpp"

Lexer::Lexer(std::string src):
        src(src){}

Lexer::~Lexer(){}

// 获取下一个 token
void Lexer::next() {
    char* last_pos;
    int hash;
    while(token = *src) {
        ++src;
        if((token >= 'a' && token <= 'z') || (token >= 'A' && token <= 'Z') || token == '_') {
            // parse identifier
            last_pos = src - 1;
            hash = token;

            while((*src >= 'a' && *src <= 'z') || (*src >= 'A' && *src <= 'Z') || *src == '_') {
                hash = hash * 147 + *src;
                src++:
            }
        }
    }
}

// 运行
void Lexer::run() {
    this->next();
    while(this->token > 0) {
        printf("[Debug] token is %3d('%c')\n", token, token);
        this->next();
    }
}