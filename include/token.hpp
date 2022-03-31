#ifndef TOKEN_HPP_
#define TOKEN_HPP_

#include <string>

#define SYMBOL_SIZE (1024 * 8)
#define MAX_NAME_SIZE 100
#define KEYWORD_SIZE 30

struct Symbol;

enum TokenType {
    Int, Float, // 整数, 浮点数
    Idn, // 标识符
    Str, // 字符串
    Equal, NonEqual, Less, LessEqual, Great, GreatEqual, SafeEqual,
    And, And2, Or, Or2, Xor, // 逻辑运算符(AND, &&, OR, || , XOR)
    Dot, // 属性运算符
    Lp, Rp, Comma, // 界符((, ), ,)
    // 关键字
    Select, From, Where, As, Insert, Into, Values,
    Update, Delete, Join, Left, Right, Min, Max, Avg, Sum,
    Union, All, GroupBy, Having, Distinct, OrderBy, True, False, 
    Is, Not, Null,
    Invalid // 无效的 token
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
    TokenType::Null,
    TokenType::And,
    TokenType::Or,
    TokenType::Xor
};


struct TokenValue {
    double value; // token value, for Num
    // used when return a string or a symbol address for assignment
    Symbol* sym_ptr;
    char* str_ptr;
};

#endif