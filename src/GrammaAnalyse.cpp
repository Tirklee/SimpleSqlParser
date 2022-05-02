#include "GrammaAnalyse.h"

map<noterminal_symbol_t, grammaNode> getGrammaFromFile(string path)
{
	map<noterminal_symbol_t, grammaNode> ans;
	fstream stream;
	stream.open(path, ios::in);
	map<noterminal_symbol_t, grammaNode>::iterator iter;
	while(!stream.eof())
	{
		int index;
		char j;
		string left;
		string s;
		string right;
		stream >> index >> j >> left >> s;
		getline(stream, right);
		right = right.substr(1);
		noterminal_symbol_t left_symbol = noterminal_symbol_t_fromstring(left);
		if(left_symbol == noterminal_symbol_tCOUNT)
			cout << "解析产生式左侧出现错误: " << left << endl; 
		bool contains_varepsilon = false;
		rightElement ele = parseRight(right, contains_varepsilon);
		ele.index = index;

		if((iter = ans.find(left_symbol)) != ans.end())
		{
			if(contains_varepsilon)
			{
				iter->second.contains_varepsilon = true;
				iter->second.varepsilon_index = index;
			}
			else
				iter->second.rights.push_back(ele);
		}
		else
		{
			grammaNode new_node;
			if(contains_varepsilon)
				new_node.contains_varepsilon = contains_varepsilon;
			else
			{
				new_node.contains_varepsilon = false;
				new_node.rights.push_back(ele);
			}
				
			ans[left_symbol] = new_node;
		}
	}
	getLeftFirst(ans);
	for(iter=ans.begin(); iter!=ans.end(); iter++)
	{
		for(int i=0; i<iter->second.rights.size(); i++)
		{
			iter->second.rights[i].first = getFirst(iter->second.rights[i], ans);
		}
	}
	stream.close();
	return ans;
}

rightElement parseRight(string right, bool& contains_varepsilon)
{
	rightElement ele;
	vector<string> right_strings = splitString(right);
	contains_varepsilon = false;
	for(int i=0; i<right_strings.size(); i++)
	{
		noterminal_symbol_t terminal;
		if((terminal = noterminal_symbol_t_fromstring(right_strings[i])) != noterminal_symbol_tCOUNT)
		{
			ele.elements.push_back(element_t(1, terminal));
			continue;
		}
		ele.elements.push_back(parseString(right_strings[i]));
		if(right_strings[i] == "$"){contains_varepsilon=true;}
		else if(right_strings[i] == "GROUP" || right_strings[i] == "ORDER")
			i++;
	}
	return ele;
}

element_t parseString(string right)
{
	element_t element;
	if(right == "=")element = element_t(0, EQUAL);
	else if(right == "<=")element = element_t(0, SMALLER_EQUAL);
	else if(right == "<")element = element_t(0, SMALLER);
	else if(right == ">")element = element_t(0, GREATER);
	else if(right == ">=")element = element_t(0, GREATER_EQUAL);
	else if(right == "NULL")element = element_t(0, _NULL);
	else if(right == "<=>")element = element_t(0, SMALLER_EQUAL_GREATER);
	else if(right == "!=")element = element_t(0, NOT_EQUAL);
	else if(right == "(")element = element_t(0, LEFT_BRAKET);
	else if(right == ")")element = element_t(0, RIGHT_BRAKET);
	else if(right == ",")element = element_t(0, COMMA);
	else if(right == ".")element = element_t(0, DOT);
	else if(right == "$")element = element_t(0, VAREPSILON);
	else if(right == "||")element = element_t(0, OR_OR);
	else if(right == "&&")element = element_t(0, AND_AND);
	else if(right == "*")element = element_t(0, MULT);
	else if(right == "!")element = element_t(0, SIGH);
	else if(right == "-")element = element_t(0, SUB);
	else if(right == "GROUP")
	{
		element = element_t(0, GROUP_BY);
	}
	else if(right == "ORDER")
	{
		element = element_t(0, ORDER_BY);
	}
	else
	{
		terminal_symbol_t terminal = terminal_symbol_t_fromstring(right);
		if(terminal == terminal_symbol_tCOUNT)
		{
			cout << "解析产生式右侧出现错误: " << right << endl;
		}
		element = element_t(0, terminal);
	}
	return element;
}

void getLeftFirst(map<noterminal_symbol_t, grammaNode>& all_nodes)
{
	struct firstResolve
	{
		bool end;
		int resolve_len;
		int limit;
		bool* eposilon_resolve;
		firstSet set;
	};
	firstResolve resolve[100];
	map<noterminal_symbol_t, grammaNode>::iterator iter = all_nodes.begin();
	int index = 0;

	// 初始化， 将所有产生式首字符是终结符的添加
	while(iter != all_nodes.end())
	{
		resolve[iter->first].end = false;
		resolve[iter->first].resolve_len = 0;
		resolve[iter->first].eposilon_resolve = new bool[iter->second.rights.size()];
		resolve[iter->first].limit = iter->second.rights.size();
		resolve[iter->first].set.contains_varepsilon = iter->second.contains_varepsilon;
		memset(resolve[iter->first].eposilon_resolve, 0, resolve[iter->first].limit);
		for(int i=0; i<iter->second.rights.size(); i++)
		{
			if(iter->second.rights[i].elements[0].type == 0)// 首字符是终结符
			{
				resolve[iter->first].resolve_len++;
				resolve[iter->first].eposilon_resolve[i] = true;
				resolve[iter->first].set.set.insert(iter->second.rights[i].elements[0].terminal);
				if(resolve[iter->first].resolve_len == resolve[iter->first].limit)
				{
					resolve[iter->first].end = true;
				}
			}
		}
		iter++;
	}

	bool change = true;
	while(change)
	{
		change = false;
		for(iter = all_nodes.begin(); iter != all_nodes.end(); iter++)
		{
			if(resolve[iter->first].end)
				continue;
			// 判断节点的每个产生式
			for(int i=0; i<resolve[iter->first].limit; i++)
			{
				if(!resolve[iter->first].eposilon_resolve[i])
				{
					bool need_resolve = true;
					bool varepsilon = false;
					// 判断当前首字符为非终结符的规则是否可以进行解析
					set<terminal_symbol_t> tmp_first;
					for(int k=0; k<iter->second.rights[i].elements.size(); k++)
					{
						// 当前是非终结符并且该非终结没有解析完毕则无法解析该产生式
						int type = iter->second.rights[i].elements[k].type;
						if(type == 0)
						{
							tmp_first.insert(iter->second.rights[i].elements[k].terminal);
							varepsilon = false;
							break;
						}
						else
						{
							int idx = iter->second.rights[i].elements[k].noterminal;
							if(!resolve[idx].end)
							{
								need_resolve = false;
								break;
							}
							else
							{
								for(terminal_symbol_t symbol: resolve[idx].set.set)
								{
									tmp_first.insert(symbol);
								}
								varepsilon = resolve[idx].set.contains_varepsilon;
								if(!resolve[idx].set.contains_varepsilon)// 非终结符中没有$则结束
								{
									break;
								}
							}
						}
					}
					if(need_resolve)
					{
						change = true;
						resolve[iter->first].eposilon_resolve[i] = true;
						for(terminal_symbol_t symbol: tmp_first)
						{
							resolve[iter->first].set.set.insert(symbol);
						}
						resolve[iter->first].resolve_len++;
						if(resolve[iter->first].resolve_len == resolve[iter->first].limit)
							resolve[iter->first].end = true;
						if(varepsilon)
							resolve[iter->first].set.contains_varepsilon = true;
						else
							resolve[iter->first].set.contains_varepsilon = false;
					}
				}
			}
		}
	}

	for(iter=all_nodes.begin(); iter!=all_nodes.end(); iter++)
	{
		if(!resolve[iter->first].end)
		{
			cout << "first集解析出现错误, 错误符号 " << iter->first << endl;
		}
		else
		{
			iter->second.first = resolve[iter->first].set;
			if(!iter->second.first.contains_varepsilon)
				iter->second.first.contains_varepsilon = iter->second.contains_varepsilon;
		}
	}
}

firstSet getFirst(rightElement node, map<noterminal_symbol_t, grammaNode> all_nodes)
{
	firstSet ans;
	bool varepsilon = false;
	for(int i=0; i<node.elements.size(); i++)
	{
		int type = node.elements[i].type;
		if(type == 0)
		{
			ans.set.insert(node.elements[i].terminal);
			varepsilon = false;
			break;
		}
		else
		{
			noterminal_symbol_t idx = node.elements[i].noterminal;
			for(terminal_symbol_t symbol: all_nodes[idx].first.set)
			{
				ans.set.insert(symbol);
			}
			varepsilon = all_nodes[idx].first.contains_varepsilon;
			if(!all_nodes[idx].first.contains_varepsilon)// 非终结符中没有$则结束
			{
				break;
			}
			
		}
	}
	ans.contains_varepsilon = varepsilon;
	return ans;
}

void getLR1(map<noterminal_symbol_t, grammaNode> nodes, LR1Element**& table, int& width, int& height)
{
	int current_index = 0;
	vector<ClosureState*> table_template;
	ClosureState* root = new ClosureState;
	ClosureElement element;
	element.left = noterminal_symbol_t::root;
	element.right = nodes[noterminal_symbol_t::root].rights[0];
	element.end = false;
	element.split_pos = 0;
	element.first = firstSet(VAREPSILON);
	root->elements.push_back(element);
	queue<ClosureState*> st;
	st.push(root);
	while(!st.empty())
	{
		ClosureState* state = st.front();
		st.pop();
		// 求闭包
		bool change = true;
		while(change)
		{
			change = false;
			vector<ClosureElement> new_elements;
			int now_index = 0;
			for(ClosureElement ele: state->elements)
			{
				if(!ele.end)
				{
					if(ele.split_pos >= ele.right.elements.size() || ele.right.elements[ele.split_pos].type == 0)
					{
						state->elements[now_index].end = true;
						continue;
					}
					firstSet new_first;
					rightElement ahead_str;
					// first集
					for(int i=ele.split_pos+1; i<ele.right.elements.size(); i++)
					{
						ahead_str.elements.push_back(ele.right.elements[i]);
					}

					for(terminal_symbol_t symbol: ele.first.set)
					{
						element_t decorate_symbol;
						decorate_symbol.type = 0;
						decorate_symbol.value = symbol;
						ahead_str.elements.push_back(decorate_symbol);
						new_first.append(getFirst(ahead_str, nodes));
						ahead_str.elements.pop_back();
					}
					// 新的产生式
					for(int i=0; i<nodes[ele.right.elements[ele.split_pos].noterminal].rights.size(); i++)
					{
						ClosureElement new_element;
						new_element.end = false;
						new_element.first = new_first;
						new_element.left = ele.right.elements[ele.split_pos].noterminal;
						new_element.right = nodes[ele.right.elements[ele.split_pos].noterminal].rights[i];
						new_element.split_pos = 0;
						if(!checkConflict(new_element, new_elements))
							new_elements.push_back(new_element);
					}
					if(nodes[ele.right.elements[ele.split_pos].noterminal].contains_varepsilon)
					{
						// ClosureElement new_element = ele;
						// new_element.split_pos = new_element.split_pos + 1;
						// if(!checkConflict(new_element, new_elements))
						// {
						// 	new_elements.push_back(new_element);
						// }
						ClosureElement new_element;
						new_element.end = false;
						new_element.split_pos = 0;
						new_element.first = new_first;
						new_element.left = ele.right.elements[ele.split_pos].noterminal;
						new_element.right.index = nodes[ele.right.elements[ele.split_pos].noterminal].varepsilon_index;
						if(!checkConflict(new_element, new_elements))
							new_elements.push_back(new_element);
					}
					state->elements[now_index].end = true;
					change = true;
				}
				now_index++;
			}
			for(ClosureElement element: new_elements)
			{
				if(!checkConflict(element, state->elements))
					state->elements.push_back(element);
			}
		}

		// 检测是否重复
		bool repeat = false;
		for(ClosureState* state_ele: table_template)
		{
			if(stateEqual(state, state_ele))
			{
				state->parent->go[state->from_element.intValue()] = state_ele;
				repeat = true;
				break;
			}
		}
		if(repeat)continue;
		if(current_index == 76)
			cout << 1 << endl;
		state->index = current_index;
		current_index++;
		table_template.push_back(state);

		// 求go函数
		set<int> go_values;
		for(int i=0; i<state->elements.size(); i++)
		{
			// 如果点到了末尾或者当前已经找了该字符的go
			if(state->elements[i].needConclusion() || go_values.find(state->elements[i].right.elements[state->elements[i].split_pos].value + (state->elements[i].right.elements[state->elements[i].split_pos].type == 1 ? terminal_symbol_tCOUNT : 0)) != go_values.end())
			{
				continue;
			}
			// 点后面元素
			element_t element = state->elements[i].right.elements[state->elements[i].split_pos];
			int val = state->elements[i].right.elements[state->elements[i].split_pos].value + (state->elements[i].right.elements[state->elements[i].split_pos].type == 1 ? terminal_symbol_tCOUNT : 0);
			go_values.insert(val);
			ClosureState* new_state = new ClosureState;
			new_state->from_element = element;
			new_state->parent = state;
			for(int k=i; k<state->elements.size(); k++)
			{
				if(!state->elements[k].needConclusion() && state->elements[k].right.elements[state->elements[k].split_pos].intValue() == element.intValue())
				{
					ClosureElement new_element = state->elements[k];
					new_element.end = false;
					new_element.split_pos++;
					new_state->elements.push_back(new_element);
				}
			}
			state->go[element.intValue()] = new_state;
			st.push(new_state);
		}
	}
	// cout << "闭包创建完成" << endl;
	// 创建转移表
	width = noterminal_symbol_tCOUNT + terminal_symbol_tCOUNT;
	height = table_template.size();
	table = new LR1Element*[height];

	for(int i=0; i<height; i++)
	{
		table[i] = new LR1Element[width];
	}
	for(int i=0; i<height; i++)
	{
		for(int k=0; k<width; k++)
		{
			table[i][k].op = ERROR;
			table[i][k].index = 0;
		}
	}
	for(int i=0; i<height; i++)
	{
		for(auto iter=table_template[i]->go.begin(); iter!=table_template[i]->go.end(); iter++)
		{
			if(iter->first >= terminal_symbol_tCOUNT)
			{
				table[i][iter->first].op = GO;
				table[i][iter->first].index = iter->second->index;
			}
			else
			{
				table[i][iter->first].op = SHIFT;
				table[i][iter->first].index = iter->second->index;
			}
		}
		for(int k=0; k<table_template[i]->elements.size(); k++)
		{
			if(table_template[i]->elements[k].needConclusion())
			{
				if(table_template[i]->elements[k].left == noterminal_symbol_t::root && table_template[i]->elements[k].right.elements[0].noterminal == dmlStatement)
				{
					table[i][VAREPSILON].op = ACCEPT;
				}
				else
				{
					for(terminal_symbol_t symbol: table_template[i]->elements[k].first.set)
					{
						table[i][symbol].op = REDUCTION;
						table[i][symbol].index = table_template[i]->elements[k].right.index;
						table[i][symbol].left = table_template[i]->elements[k].left;
						table[i][symbol].right = table_template[i]->elements[k].right;
					}
				}
			}
		}
	}
	// 最后要free
	for(int i=0; i<table_template.size(); i++)
	{
		delete table_template[i];
	}
}

bool checkConflict(ClosureElement element, vector<ClosureElement>& elements)
{
	for(int i=0; i<elements.size(); i++)
	{
		if(elements[i] == element)
		{
			elements[i].first.append(element.first);
			return true;
		}
	}
	return false;
}

void generateLR1Table(string gramma_path, string save_path, LR1Element**& table, int& width, int& height)
{
	map<noterminal_symbol_t, grammaNode> nodes = getGrammaFromFile(gramma_path);
	getLR1(nodes, table, width, height);
	saveTableToFile(save_path, table, width, height);
}

bool saveTableToFile(string path, LR1Element** table, int width, int height)
{
	fstream stream;
	stream.open(path, ios::ate | ios::out);
	stream << height << "," << width << endl;
	for(int i=0; i<height; i++)
	{
		for(int k=0; k<width; k++)
		{
			stream << table[i][k].serialize() << endl;
		}
	}
	return true;
}

void LoadLR1FromFile(string path, LR1Element**& table, int& width, int& height)
{
	ifstream fin;
	fin.open(path, ios::in);
	if(!fin)
	{
		generateLR1Table("doc/gramma.txt", path, table, width, height);
	}
	else
	{
		char c;
		string s;
		fin >> height >> c >> width;
		getline(fin, s);
		table = new LR1Element*[height];
		for(int i=0; i<height; i++)
		{
			table[i] = new LR1Element[width];
		}
		for(int i=0; i<height; i++)
		{
			for(int k=0; k<width; k++)
			{
				getline(fin, s);
				LR1Element ele;
				ele.deserialize(s);
				table[i][k] = ele;
			}
		}
	}
}

bool stateEqual(ClosureState* i, ClosureState* other)
{
	for(ClosureElement element: other->elements)
	{
		bool equal = false;
		for(ClosureElement mine: i->elements)
		{
			if(element == mine && element.first == mine.first)
			{
				equal = true;
				break;
			}
		}
		if(!equal)return false;
	}
	return true;
}

void printTableToFile(string path, LR1Element** table, int width, int height)
{
	fstream stream;
	stream.open(path, ios::ate | ios::out);
	for(int i=0; i<width; i++)
	{
		if(i < terminal_symbol_tCOUNT)
			stream << terminal_symbol_t_tostring(terminal_symbol_t(i));
		else
			stream << noterminal_symbol_t_tostring(noterminal_symbol_t(i-terminal_symbol_tCOUNT));
		if(i != width - 1)
			stream << ",";
		else
			stream << endl;
	}
	for(int i=0; i<height; i++)
	{
		for(int k=0; k<width; k++)
		{
			string s;
			switch (table[i][k].op)
			{
			case SHIFT: s.append("s" + to_string(table[i][k].index));break;
			case GO: s.append(to_string(table[i][k].index));break;
			case REDUCTION: s.append("r" + to_string(table[i][k].right.index));break;
			case ACCEPT: s.append("ACCEPT");break;
			case ERROR: s.append("ERROR");break;
			default:
				break;
			}
			if(k == width-1)
				stream << s;
			else
				stream << s << ",";
		}
		stream << endl;
	}
	stream.close();
}

void LR1Table::init(string file)
{
	LoadLR1FromFile(file, table, width, height);
	printTable();
}

bool LR1Table::parseToken(element_t token)
{
//    // 如果不是移进操作说明之前出现了错误
//    if(table[state.top()][token.intValue()].op != SHIFT)
//        return false;
//    // 先将符号移入
//    element.push(token);
//    state.push(table[state.top()][token.intValue()].index);
//    element_t temp_element = token;
    while(true)
    {
        int temp_state = state.top();
    //    cout<<table[temp_state][token.intValue()].op<<' '<<temp_state<<' '<<token.intValue()<<endl;
        string stack_top, input_token;
        if(element.top().type == 0) // terminal
            stack_top = terminal_symbol_t_tostring(element.top().terminal);
        else
            stack_top = noterminal_symbol_t_tostring(element.top().noterminal);
        if(token.type == 0) // terminal
            input_token = terminal_symbol_t_tostring(token.terminal);
        else
            input_token = noterminal_symbol_t_tostring(token.noterminal);
        if(table[temp_state][token.intValue()].op == REDUCTION)// 规约
        {
            cout<<num++<<'\t'<<table[temp_state][token.intValue()].right.index<<'\t'<<stack_top<<'#'<<input_token<<'\t'<<"reduction"<<endl;
            int n = table[temp_state][token.intValue()].right.elements.size();
            for(int i=n-1;i>=0;i--)
            {
                element.pop();
                state.pop();
            }
            element_t temp_element = element_t(1,table[temp_state][token.intValue()].left);
            element.push(temp_element);
            if(table[state.top()][temp_element.intValue()].op==GO)
            {
                state.push(table[state.top()][temp_element.intValue()].index);
            }

        }
        else if(table[temp_state][token.intValue()].op == SHIFT)// 移进
        {
            cout<<num++<<'\t'<<'/'<<'\t'<<stack_top<<'#'<<input_token<<'\t'<<"move"<<endl;
            element.push(token);
            state.push(table[temp_state][token.intValue()].index);
            string stack_top, input_token;

            return true;
        }
        else if(table[temp_state][token.intValue()].op == ACCEPT)
        {
            return true;
        }
        else if(table[temp_state][token.intValue()].op == ERROR)
        {
            return false;
        }

    }
    return true;
}

void LR1Table::printTable(string path)
{
	printTableToFile(path, table, width, height);
}

void LR1Table::reset()
{
    state.push(0);
	element_t stack_base;
	stack_base.type = 0;
	stack_base.terminal = VAREPSILON;
	element.push(stack_base);
    num = 0;
}