#ifndef TOKEN_HPP_
#define TOKEN_HPP_

#include <string>

#define SYMBOL_SIZE (1024 * 8)
#define MAX_NAME_SIZE (100)

enum TokenType {
    Int, Float, Idn, Str,
    Eq, Ne, Lt, Le, Gt, Ge,
    And, Or, Xor   
};

struct Symbol {
    // 符号类型: Num,
    TokenType type;
    char name[MAX_NAME_SIZE];
    double value;
    int levelNum;
};

struct TokenValue {
    double value; // token value, for Num
    Symbol* ptr; // used when return a string or a symbol address for assignment
};



enum Errors {

};
#endif