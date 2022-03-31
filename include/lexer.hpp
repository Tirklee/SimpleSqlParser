#ifndef LEXER_HPP__
#define LEXER_HPP__

#include <memory>
#include <vector>
#include <string.h>
#include <optional>
#include "token.hpp"
#include "symbol.hpp"

class Lexer
{
private:
    char* src; //  源码
    int token;
    TokenType token_type;
    TokenValue token_val;
    std::vector<Symbol> symtab; // 符号表
    std::optional<char*> name;
public:
    void add_keywords();
    char* lookupn(int size);
    char* lookdown(int size);
    void next();
    void run(char* src);
    Lexer();
    ~Lexer();
};




#endif