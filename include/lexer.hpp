#ifndef LEXER_HPP__
#define LEXER_HPP__

#include <memory>
#include <vector>
#include <string.h>
#include "token.hpp"
#include "symbol.hpp"

class Lexer
{
private:
    char* src; //  源码
    TokenType token_type;
    int token; // 当前的 token
    TokenValue token_val;
    std::vector<Symbol> symtab; // 符号表
public:
    void add_keywords();
    void next();
    void run(char* src);
    Lexer();
    ~Lexer();
};




#endif