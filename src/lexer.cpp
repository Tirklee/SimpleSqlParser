
#include "lexer.hpp"

Lexer::Lexer(){}

Lexer::~Lexer(){}

// 获取下一个 token
void Lexer::next() {
    char* last_pos;
    int hash;
    while(token = *src) {
        ++src;
        if((token >= 'a' && token <= 'z') || (token >= 'A' && token <= 'Z') || token == '_') {
            last_pos = src - 1;
            // 获取符号名
            char nameBuffer[100];
            nameBuffer[0] = token;
            while((*src >= 'a' && *src <= 'z') || (*src >= 'A' && *src <= 'Z') || *src == '_') {
                nameBuffer[src - last_pos] = *src;
                src++;
            }
            nameBuffer[src - last_pos] = 0;
            // 从符号表中查找是否有对应的符号名
            for(auto sym: this->symtab) {
                if(strcmp(sym.name, nameBuffer) == 0) {

                }
            }
        }else if(token >= '0' && token <= '9') {
            this->token_val.value = (double)token - '0';
            while(*src >= '0' && *src <= '9') {
                this->token_val.value = this->token_val.value * 10.0 + ((double)(*src++) - '0');
            }
            if(*src == '.') {
                this->token_type = TokenType::Float;
                // 浮点数
                src++;
                int countDig = 1;
                while(*src >= '0' && *src <= '9') {
                    this->token_val.value = this->token_val.value + ((double)(*src++) - '0')/(10.0 * countDig);
                    countDig++;
                }
            }else {
                this->token_type = TokenType::Int;
            }
            return;
        }
    }
}


// 运行
void Lexer::run(char* src) {
    this->src = src;
    this->next();
    while(this->token > 0) {
        switch(this->token_type) {
            case TokenType::Float: 
                printf("Token Type: Float, Value: %d\n", this->token_val.value);
                break;
            case TokenType::Int:
                printf("Token Type: Int, Value: %d\n", this->token_val.value);
        }
        this->next();
    }
}