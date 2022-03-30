#ifndef TOKEN_HPP_
#define TOKEN_HPP_

#include <string>

#define SYMBOL_SIZE (1024 * 8)
#define MAX_NAME_SIZE 100
#define KEYWORD_SIZE 27

struct Symbol;

enum TokenType {
    Int, Float, Idn, Str,
    Eq, Ne, Lt, Le, Gt, Ge,
    And, Or, Xor,
    Select, From, Where, As, Insert, Into, Values,
    Update, Delete, Join, Left, Right, Min, Max, Avg, Sum,
    Union, All, GroupBy, Having, Distinct, OrderBy, True, False, 
    Is, Not, Null

};

const TokenType keywords[] = {
    TokenType::Select,
    TokenType::From,
    TokenType::Where,
    TokenType::As, 
    TokenType::Insert,
    TokenType::Into, 
    TokenType::Values,
    TokenType::Update,
    TokenType::Delete,
    TokenType::Join,
    TokenType::Left,
    TokenType::Right,
    TokenType::Min,
    TokenType::Max,
    TokenType::Avg,
    TokenType::Sum,
    TokenType::Union,
    TokenType::All,
    TokenType::GroupBy,
    TokenType::Having,
    TokenType::Distinct,
    TokenType::OrderBy,
    TokenType::True,
    TokenType::False,
    TokenType::Is,
    TokenType::Not,
    TokenType::Null
};


struct TokenValue {
    double value; // token value, for Num
    Symbol* ptr; // used when return a string or a symbol address for assignment
};

#endif