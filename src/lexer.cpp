
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
        this->next(0);
        this->symtab[this->symtab.size() - 1].type = keywords[i];
    }
    
}

void Lexer::init() {
    std::vector<std::string> keywords = {
        "SELECT", "FROM", "WHERE", "AS", "INSERT", "INTO", "VALUES", "VALUE", "DEFAULT",
        "UPDATE", "SET", "DELETE", "JOIN", "LEFT", "RIGHT", "ON", "MIN", "MAX", "AVG", "SUM",
        "UNION", "ALL", "GROUP BY", "HAVING", "DISTINCT", "ORDER BY", "TRUE", "FALSE", "UNKNOWN",
        "IS", "NULL", "AND", "OR", "XOR", "NOT"
    };
    this->keywords_name = keywords;
    this->add_keywords();
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

void Lexer::add_idn_to_token(Symbol symbol) {
    if(std::find(this->keywords_name.begin(), this->keywords_name.end(), symbol.name) != this->keywords_name.end()) {
        // 发现关键字, 直接将关键字的名字加入到 parser_token type 中
        this->parser_token.type = symbol.name;
    }else {
        // 此时为标识符
        this->parser_token.type = "IDN";
        this->parser_token.str.emplace(symbol.name);
        this->parser_token.value.emplace(symbol.value);
    }
}

// 获取下一个 token
bool Lexer::next(bool show) {
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
                    goto OUT;
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
                    // 将标识符添加到 parser_token 中
                    this->add_idn_to_token(sym);
                    goto OUT;
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
            // 将标识符加入到 parser_token 中
            this->add_idn_to_token(symbol);
            goto OUT;
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
                this->parser_token.type = "FLOAT";
            }else {
                this->token_type = TokenType::Int;
                this->parser_token.type = "INT";
            }
            this->parser_token.value.emplace(this->token_val.value);
            goto OUT;
        }else if(token == '"') {
            // 字符串
            int size = 0;
            while(*src != token) {
                if(*src == 0) {
                    this->token_type = TokenType::Invalid;
                    goto OUT;
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
            // 为 parser 添加 token
            this->parser_token.type = "STRING";
            this->parser_token.str.emplace(str);
            goto OUT;
        }else if(token == '*'){
            this->parser_token.type = "*";
            this->token_type = TokenType::WildCard;
            goto OUT;
        }else if(token == '='){
            // 等于符号
            this->parser_token.type = "=";
            this->token_type = TokenType::Equal;
            goto OUT;
        }else if(token == '!'){
            // 不等于符号
            if(*src == '=') {
                src++;
                this->parser_token.type = "!=";
                this->token_type = TokenType::NonEqual;
            }else{
                this->parser_token.type = "!";
                this->token_type = TokenType::Not2;
            }
            goto OUT;
        }else if(token == '<') {
            // < / <= / <=>
            if(*src == '=') {
                src++;
                if(*src == '>') {
                    src++;
                    this->parser_token.type = "<=>";
                    this->token_type = TokenType::SafeEqual;
                }else {
                    this->parser_token.type = "<=";
                    this->token_type = TokenType::LessEqual;
                }
            }else {
                this->parser_token.type = "<";
                this->token_type = TokenType::Less;
            }
            goto OUT;
        }else if(token == '>') {
            // > / >=
            if(*src == '=') {
                src++;
                this->parser_token.type = ">=";
                this->token_type = TokenType::GreatEqual;
            }else {
                this->parser_token.type = ">";
                this->token_type = TokenType::Great;
            }
            goto OUT;
        }else if(token == '|') {
            if(*src == '|') {
                src++;
                this->parser_token.type = "||";
                this->token_type = TokenType::Or2;
            }else {
                this->parser_token.type = "INVALID";
                this->token_type = TokenType::Invalid;
            }
            goto OUT;
        }else if(token == '&') {
            if(*src == '&') {
                src++;
                this->parser_token.type = "&&";
                this->token_type = TokenType::And2;
            }else{
                this->parser_token.type = "INVALID";
                this->token_type = TokenType::Invalid;
            }
            goto OUT;
        }else if(token == '-'){
            this->parser_token.type = "-";
            this->token_type = TokenType::Sub;
            goto OUT;
        }else if(token == '.'){
            this->parser_token.type = ".";
            this->token_type = TokenType::Dot;
            goto OUT;
        }else if(token == '('){
            this->parser_token.type = "(";
            this->token_type = TokenType::Lp;
            goto OUT;
        }else if(token == ')'){
            this->parser_token.type = ")";
            this->token_type = TokenType::Rp;
            goto OUT;
        }else if(this->token == ','){
            this->parser_token.type = ",";
            this->token_type = TokenType::Comma;
            goto OUT;
        }else if(token == ' ' || token == '\t' || token == ';' || token == '\n') {}
        else if(token == '\\'){}
        else{
            this->parser_token.type = "INVALID";
            this->token_type = TokenType::Invalid;
            printf("[Error] Unexpected token: %c\n", token);
        }
    }
OUT:;
	if(show)
	{
		return printToken();
	}
	return true;
}

Token Lexer::get_token() {
    return this->parser_token;
}

int Lexer::get_current_token_value() {
    return this->token;
}

// 运行
void Lexer::run(char* src) {
    this->src = src;
}

bool Lexer::printToken()
{
	if(this->token > 0) {
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
#ifdef DEBUG
        printf("[Debug] parser: ");
        printf("type: %s ", this->parser_token.type.c_str());
        if(this->parser_token.value.has_value()) {
            printf("value: %lf ", this->parser_token.value.value());
        }
        if(this->parser_token.str.has_value()) {
            printf("str: %s\n", this->parser_token.str.value().c_str());
        }
#endif
	return true;
    }
	return false;
} 

// StateParser::StateParser(string file_path) : path(file_path)
// {

// }

// void StateParser::showResult(string file_path)
// {
// 	parseFile();
// 	determine();
// 	minimize();
// 	// 将最小化结果输出到文件中
// }

// void StateParser::parseFile()
// {

// }

// void StateParser::determine()
// {

// 	vector<ClosureState*> states;
// 	int index = 0;
// 	ClosureState* base_state = new ClosureState;
// 	base_state->contain_state.push_back(m_states[0]);
// 	stack<ClosureState*> st;
// 	st.push(base_state);
// 	while(!st.empty())
// 	{
// 		ClosureState* state = st.top();
// 		st.pop();
// 		state->index = index;
// 		index++;
// 		// 处理空
// 		varepsilonClosure(state);
// 		unordered_map<char, vector<State*>> go;
// 		for(int i = 0; i < state->contain_state.size(); i++)
// 		{
// 			State* now_state = state->contain_state[i];
// 			for(int k = 0; k < now_state->go.size(); k++)
// 			{
// 				pair<vector<char>, State*> current_element = now_state->go[k];
// 				for(int j = 0; j < current_element.first.size(); j++)
// 				{
// 					if(go.find(current_element.first[j]) == go.end())
// 					{
// 						vector<State*> new_state;
// 						new_state.push_back(current_element.second);
// 						go[current_element.first[j]] = new_state;
// 					}
// 					else
// 					{
// 						if(!checkStateConflict(go[current_element.first[j]], current_element.second))
// 							go[current_element.first[j]].push_back(current_element.second);
// 					}
// 				}
// 			}
// 		}
// 		// go是当前状态经过符号后跳转到的状态
// 		// 判断是否重复，如果没有重复则认为是一个新的状态
// 		for(auto iter=go.begin(); iter!=go.end(); iter++)
// 		{
// 			if(!checkClosureStateConflict(states, iter->second))
// 			{
// 				ClosureState* new_state = new ClosureState;
// 				new_state->contain_state = iter->second;
// 				new_state->from_element = iter->first;
// 				new_state->parent = state;
// 				state->go.push_back(pair(iter->first, new_state));
// 				st.push(new_state);
// 			}
// 		}
// 	}

// 	// TODO: 确定is_begin和is_end的值，并转化成DetermineState*
// }

// bool StateParser::checkStateConflict(vector<State*> states, State* state)
// {
// 	for(int i=0; i<states.size(); i++)
// 	{
// 		if(state->index == states[i]->index)
// 			return true;
// 	}
// 	return false;
// }

// bool StateParser::checkClosureStateConflict(vector<ClosureState*> states, vector<State*> state)
// {
// 	for(int i=0; i<states.size(); i++)
// 	{
// 		for(int k=0; k<states[i]->contain_state.size(); k++)
// 		{
// 			int j;
// 			for(j=0; j<state.size(); j++)
// 			{
// 				if(states[i]->contain_state[k]->index == state[j]->index)
// 				{
// 					break;
// 				}
// 			}
// 			if(j == state.size())
// 			{
// 				return false;
// 			}
// 		}
// 	}
// 	return true;
// }

// void StateParser::varepsilonClosure(ClosureState* before)
// {

// } 

// void StateParser::minimize()
// {
	
// }