#ifndef LEXER_HPP__
#define LEXER_HPP__

#include <string>
#include <memory>

class Lexer
{
private:
    std::string src; //  源码
    int token; // 当前的 token
    int token_val; // token 的值
    std::unique_ptr<int> symbols; // 符号表
public:
    void next();
    void run();
    Lexer(std::string src);
    ~Lexer();
};




#endif