#ifndef TOKEN_HPP_
#define TOKEN_HPP_

#include <string>

#define SYMBOL_SIZE (1024 * 8)
#define MAX_NAME_SIZE 100
// #define KEYWORD_SIZE 30

struct Symbol;

enum TokenType {
    Int, Float, // 整数, 浮点数
    Idn, // 标识符
    Str, // 字符串
    Equal, NonEqual, Less, LessEqual, Great, GreatEqual, SafeEqual, // 比较运算符
    And, And2, Or, Or2, Xor, Not, Not2,// 逻辑运算符(AND, &&, OR, || , XOR)
    Sub, // 算术运算符
    Dot, // 属性运算符
    Lp, Rp, Comma, // 界符((, ), ,)
    // 关键字
    Select, From, Where, As, WildCard,// 查询表达式
    Insert, Into, Values, Value, Default, // 插入表达式
    Update, Set,  // 更新表达式
    Delete, // 删除表达式
    Join, Left, Right, On, // 连接操作
    Min, Max, Avg, Sum, // 聚合操作
    Union, All, // 集合操作
    GroupBy, Having, Distinct, OrderBy, // 组操作 
    True, False, Unknown, Is, Null, // 条件语句
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
    TokenType::Value,
    TokenType::Default,
    TokenType::Update,
    TokenType::Set,
    TokenType::Delete,
    TokenType::Join,
    TokenType::Left,
    TokenType::Right,
    TokenType::On,
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
    TokenType::Unknown,
    TokenType::Is,
    TokenType::Null,
    TokenType::And,
    TokenType::Or,
    TokenType::Xor,
    TokenType::Not
};




struct TokenValue {
    double value; // token value, for Num
    // used when return a string or a symbol address for assignment
    Symbol* sym_ptr;
    char* str_ptr;
};

#endif