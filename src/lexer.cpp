
#include "lexer.hpp"

Lexer::Lexer(){}

Lexer::~Lexer(){}

void Lexer::add_keywords() {
    const char* src = "SELECT FROM WHERE AS INSERT INTO VALUES \
                UPDATE DELETE JOIN LEFT RIGHT MIN MAX AVG SUM \
                UNION ALL GROUPBY HAVING DISTINCT ORDERBY TRUE FALSE \
                IS NOT NULL";
    this->src = (char*)src;
    for(size_t i = 0; i < KEYWORD_SIZE; i++) {
        this->next();
        
        this->symtab[this->symtab.size() - 1].type = keywords[i];
    }
    
}

// 获取下一个 token
void Lexer::next() {
    char* last_pos;
    while((token = *src) && *src != 0) {
        ++src;
        if((token >= 'a' && token <= 'z') || (token >= 'A' && token <= 'Z') || token == '_') {
            last_pos = src - 1;
            // 获取符号名
            char nameBuffer[100];
            nameBuffer[0] = token;
            while((*src >= 'a' && *src <= 'z') || (*src >= 'A' && *src <= 'Z') || (*src >= '0' && *src <= '9') ||*src == '_') {
                nameBuffer[src - last_pos] = *src;
                src++;
            }
            nameBuffer[src - last_pos] = 0;
            // 从符号表中查找是否有对应的符号名
            for(auto sym: this->symtab) {
                if(strcmp(sym.name, nameBuffer) == 0) {
                    this->token_val.sym_ptr = &sym;
                    return;
                }
            }
            printf("[Debug] next(): name: %s\n", nameBuffer);
            // 如果未发现的话则需要构建符号
            Symbol symbol;
            strcpy(symbol.name, nameBuffer);
            symbol.type = TokenType::Idn;
            symbol.value = 0.0;
            this->symtab.push_back(symbol);
            this->token_val.sym_ptr = &this->symtab[this->symtab.size() - 1];
            return;
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
        }else if(token == '"') {
            // 字符串
            int size = 0;
            while(*src != token) {
                if(*src == 0) {
                    this->token_type = TokenType::Invalid;
                    return;
                }
                src++;
                size++;
            }
            char* str = new char[size + 5];
            memcpy(str, src - size, size);
            this->token_type = TokenType::Str;
            this->token_val.str_ptr = str;
            return;
        }else if(token == '='){
            // 等于符号
            this->token_type = TokenType::Equal;
            return;
        }else if(token == '!'){
            // 不等于符号
            if(*src == '=') {
                src++;
                this->token_type = TokenType::NonEqual;
            }else{
                this->token_type = TokenType::Invalid;
            }
            return;
        }else if(token == '<') {
            // < / <= / <=>
            if(*src == '=') {
                src++;
                if(*src == '>') {
                    src++;
                    this->token_type = TokenType::SafeEqual;
                }else {
                    this->token_type = TokenType::LessEqual;
                }
            }else {
                this->token_type = TokenType::Less;
            }
            return;
        }else if(token == '>') {
            // > / >=
            if(*src == '=') {
                src++;
                this->token_type = TokenType::GreatEqual;
            }else {
                this->token_type = TokenType::Great;
            }
            return;
        }else if(token == '|') {
            if(*src == '|') {
                src++;
                this->token_type = TokenType::Or2;
            }else {
                this->token_type = TokenType::Invalid;
            }
            return;
        }else if(token == '&') {
            if(*src == '&') {
                src++;
                this->token_type = TokenType::And2;
            }else{
                this->token_type = TokenType::Invalid;
            }
            return;
        }else if(token == '.'){
            this->token_type = TokenType::Dot;
            return;
        }else if(token == ' ' || token == '\t') {}
        else if(token == '\\'){}
        else{
            printf("[Error] Unexpected token: %c\n", token);
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
                printf("FLOAT\t<FLOAT, %lf>\n", this->token_val.value);
                break;
            case TokenType::Int:
                printf("INT\t<INT, %d>\n", (int)this->token_val.value);
                break;
            case TokenType::Str:
                printf("Str\t<Str, %lu>\n", (size_t)this->token_val.str_ptr);
            case TokenType::Equal:
                printf("=\t<OP, 1>\n");
                break;
            case TokenType::Great:
                printf(">\t<OP, 2>\n");
                break;
            case TokenType::Less:
                printf("<\t<OP, 3>\n");
                break;
            case TokenType::GreatEqual:
                printf(">=\t<OP, 4>\n");
                break;
            case TokenType::LessEqual:
                printf("<=\t<OP, 5>\n");
                break;
            case TokenType::NonEqual:
                printf("!=\t<OP, 6>\n");
                break; 
            case TokenType::SafeEqual:
                printf("<=>\t<OP, 7>\n");
                break;
            case TokenType::And:
                printf("AND\t<OP, 8>\n");
                break;
            case TokenType::And2:
                printf("&&\t<OP, 9>\n");
                break;
            case TokenType::Or:
                printf("OR\t<OP, 10>\n");
                break;
            case TokenType::Or2:
                printf("||\t<OP, 11>\n");
                break;
            case TokenType::Xor:
                printf("XOR\t<OP, 12>\n");
                break;
            case TokenType::Dot:
                printf(".\t<OP, 13>\n");
            case TokenType::Invalid:
                printf("[Error] Unexpected token\n");
                exit(-1);
            default:
                printf("[Error] Fail to find token\n");
        }
        this->next();
    }
}