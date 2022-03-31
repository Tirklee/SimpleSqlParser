#ifndef SYMBOL_HPP_
#define SYMBOL_HPP_

#include "token.hpp"
struct Symbol {
    // Symbol Type: Int, Float, Str,...
    TokenType type;
    char name[MAX_NAME_SIZE];
    double value;
};

#endif