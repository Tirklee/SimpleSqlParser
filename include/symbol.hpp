#ifndef SYMBOL_HPP_
#define SYMBOL_HPP_

#include "token.hpp"
struct Symbol {
    // 符号类型: Num,
    TokenType type;
    char name[MAX_NAME_SIZE];
    double value;
    int levelNum;
};

#endif