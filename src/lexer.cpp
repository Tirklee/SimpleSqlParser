
#include "lexer.hpp"

Lexer::Lexer(){}

Lexer::~Lexer(){}

void Lexer::add_keywords() {
    const char* src = "SELECT FROM WHERE AS INSERT INTO VALUES VALUE DEFAULT\
                UPDATE SET DELETE JOIN LEFT RIGHT ON MIN MAX AVG SUM \
                UNION ALL GROUP BY HAVING DISTINCT ORDER BY TRUE FALSE UNKNOWN \
                IS NULL AND OR XOR NOT";
    this->src = (char*)src;
    size_t keyword_size = sizeof(keywords)/sizeof(keywords[0]);
    for(size_t i = 0; i < keyword_size; i++) {
        this->next();
        this->symtab[this->symtab.size() - 1].type = keywords[i];
    }
    
}

// 向前查看 n 个字符, 用于对于有相同部分的标识符进行识别
char* Lexer::lookupn(int size) {
    char* ptr = src - size;
    char* buf = new char[size + 5];
    memcpy(buf, ptr, size);
    buf[size] = '\0';
    return buf;
}

char* Lexer::lookdown(int size) {
    char* buf = new char[size + 5];
    memcpy(buf, src, size);
    buf[size] = '\0';
    return buf;
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
            while((*src >= 'a' && *src <= 'z') || (*src >= 'A' && *src <= 'Z') || (*src >= '0' && *src <= '9') || *src == '_') {
                nameBuffer[src - last_pos] = *src;
                src++;
            }
            nameBuffer[src - last_pos] = 0;
            if(strcmp(nameBuffer, "ORDER") == 0 || strcmp(nameBuffer, "GROUP") == 0) {
                char* buf = this->lookdown(3);
                if(strcmp(buf, " BY") == 0) {
                    // 将指针向后移 3 位
                    src += 3;
                    memcpy(nameBuffer + (src - last_pos), buf, 3);
                }else {
                    this->token_type = TokenType::Invalid;
                    return;
                }
            }
            // 从符号表中查找是否有对应的符号名
            for(auto sym: this->symtab) {
                if(strcmp(sym.name, nameBuffer) == 0) {
                    this->token_val.sym_ptr = &sym;
                    this->token_type = sym.type;
                    char* name = new char[MAX_NAME_SIZE];
                    strcpy(name, nameBuffer);
                    this->name = std::make_optional(name);
                    return;
                }
            }
#ifdef DEBUG
            printf("[Debug] next(): name: %s\n", nameBuffer);
#endif
            // 如果未发现的话则需要构建符号
            Symbol symbol;
            strcpy(symbol.name, nameBuffer);
            symbol.type = TokenType::Idn;
            symbol.value = 0.0;
            this->symtab.push_back(symbol);
            this->token_val.sym_ptr = &this->symtab[this->symtab.size() - 1];
            this->token_type = TokenType::Idn;
            char* name = new char[MAX_NAME_SIZE];
            strcpy(name, nameBuffer);
            this->name = std::make_optional(name);
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
            // 将对应的字符串放入地址中并将其存入符号表中
            char* str = new char[size + 5];
            memcpy(str, src - size, size);
            str[size] = '\0';
            this->token_type = TokenType::Str;
            this->token_val.str_ptr = str;
            src++;
            return;
        }else if(token == '*'){
            this->token_type = TokenType::WildCard;
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
                this->token_type = TokenType::Not2;
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
        }else if(token == '-'){
            this->token_type = TokenType::Sub;
            return;
        }else if(token == '.'){
            this->token_type = TokenType::Dot;
            return;
        }else if(token == '('){
            this->token_type = TokenType::Lp;
            return;
        }else if(token == ')'){
            this->token_type = TokenType::Rp;
            return;
        }else if(this->token == ','){
            this->token_type = TokenType::Comma;
            return;
        }else if(token == ' ' || token == '\t' || token == ';' || token == '\n') {}
        else if(token == '\\'){}
        else{
            this->token_type = TokenType::Invalid;
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
                printf("STR\t<Str, 0x%lx>\n", (size_t)this->token_val.str_ptr);
                printf("%s\n", (char*)this->token_val.str_ptr);
                break;
            case TokenType::Idn:
                if(this->name.has_value()) {
                    printf("%s\t<IDN, %s>\n", this->name.value(), this->name.value());
                }
                break;
            case TokenType::Lp:
                printf("(\t<SE, 1>\n");
                break;
            case TokenType::Rp:
                printf(")\t<SE, 2>\n");
                break;
            case TokenType::Comma:
                printf(",\t<SE, 3>\n");
                break;
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
            case TokenType::Not:
                printf("NOT\t<OP, 13>\n");
                break;
            case TokenType::Not2:
                printf("!\t<OP, 14>\n");
                break;
            case TokenType::Sub:
                printf("-\t<KW, 15>\n");
                break;
            case TokenType::Dot:
                printf(".\t<OP, 16>\n");
                break;
            case TokenType::Select:
                printf("SELECT\t<KW, 1>\n");
                break; 
            case TokenType::From:
                printf("FROM\t<KW, 2>\n");
                break;
            case TokenType::Where:
                printf("WHERE\t<KW, 3>\n");
                break;
            case TokenType::As:
                printf("AS\t<KW, 4>\n");
                break;
            case TokenType::WildCard:
                printf("*\t<KW, 5>\n");
                break;
            case TokenType::Insert:
                printf("INSERT\t<KW, 6>\n");
                break;
            case TokenType::Into:
                printf("INTO\t<KW, 7>\n");
                break;
            case TokenType::Values:
                printf("VALUES\t<KW, 8>\n");
                break;
            case TokenType::Value:
                printf("VALUE\t<KW, 9>\n");
                break;
            case TokenType::Default:
                printf("DEFAULT\t<KW, 10>\n");
                break;
            case TokenType::Update:
                printf("UPDATE\t<KW, 11>\n");
                break;
            case TokenType::Set:
                printf("SET\t<KW, 12>\n");
                break;
            case TokenType::Delete:
                printf("DELECTE\t<KW, 13>\n");
                break;
            case TokenType::Join:
                printf("JOIN\t<KW, 14>\n");
                break;
            case TokenType::Left:
                printf("LEFT\t<KW, 15>\n");
                break;
            case TokenType::Right:
                printf("RIGHT\t<KW, 16>\n");
                break;
            case TokenType::On:
                printf("ON\t<KW, 17>\n");
                break;
            case TokenType::Min:
                printf("MIN\t<KW, 18>\n");
                break;
            case TokenType::Max:
                printf("MAX\t<KW, 19>\n");
                break;
            case TokenType::Avg:
                printf("AVG\t<KW, 20>\n");
                break;
            case TokenType::Sum:
                printf("SUM\t<KW, 21>\n");
                break;
            case TokenType::Union:
                printf("UNION\t<KW, 22>\n");
                break;
            case TokenType::All:
                printf("ALL\t<KW, 23>\n");
                break;
            case TokenType::GroupBy:
                printf("GROUP BY\t<KW, 24>\n");
                break;
            case TokenType::Having:
                printf("HAVING\t<KW, 25>\n");
                break;
            case TokenType::Distinct:
                printf("DISTINCT\t<KW, 26>\n");
                break;
            case TokenType::OrderBy:
                printf("ORDER BY\t<KW, 27>\n");
                break;
            case TokenType::True:
                printf("TRUE\t<KW, 28>\n");
                break;
            case TokenType::False:
                printf("FALSE\t<KW, 29>\n");
                break;
            case TokenType::Unknown:
                printf("UNKNOWN\t<KW, 30>\n");
                break;
            case TokenType::Is:
                printf("IS\t<KW, 31>\n");
                break;
            case TokenType::Null:
                printf("NULL\t<KW, 32>\n");
                break;
            case TokenType::Invalid:
                printf("[Error] Unexpected token: %c\n", token);
                exit(-1);
            default:
                printf("[Error] Fail to find token\n");
        }
        this->next();
    }
}