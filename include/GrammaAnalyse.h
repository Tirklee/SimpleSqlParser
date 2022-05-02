#ifndef __GRAMMA_ANALYSE_H
#define __GRAMMA_ANALYSE_H

#include<vector>
#include "common.h"
#include<fstream>
#include<string>
#include<cstring>
#include<map>
#include<set>
#include<queue>
#include<stack>
#include "utils.h"
using namespace std;

struct element_t
{
	element_t(){}
	element_t(int type, int value)
	{
		this->type = type;
		if(type == 0)
		{
			terminal = (terminal_symbol_t)value;
		}
		else
		{
			noterminal = (noterminal_symbol_t)value;
		}
		
	}
	int type; // 0 terminal
	union 
	{
		int value;
		noterminal_symbol_t noterminal;
		terminal_symbol_t terminal;
	};
	int intValue() const
	{
		return value + (type == 1 ? terminal_symbol_tCOUNT: 0);
	}

	bool operator==(const element_t& e)
	{
		return type == e.type && value == e.value;
	}
};

struct firstSet
{
	firstSet(){}
	firstSet(terminal_symbol_t symbol){set.insert(symbol);contains_varepsilon=false;}
	set<terminal_symbol_t> set;
	bool contains_varepsilon; // 是否存在空字符串$
	void append(firstSet first_set)
	{
		for(terminal_symbol_t symbol: first_set.set)
		{
			set.insert(symbol);
		}
	}
	bool operator==(const firstSet& other)
	{
		return set == other.set;
	}
};

struct followSet
{
	set<terminal_symbol_t> set;
};

struct rightElement
{
	int index; // 归约编号
	vector<element_t> elements;
	firstSet first;
	bool operator==(const rightElement& other)
	{
		return index == other.index;
	}
};

struct grammaNode
{
	vector<rightElement> rights;
	bool contains_varepsilon; // 是否存在空字符串$
	int varepsilon_index;
	firstSet first;
	followSet follow;
};


struct LR1Element
{
	operation_t op;
	int index;
	noterminal_symbol_t left;
	rightElement right;
	string serialize()
	{
		string ans;
		ans.append(to_string(op) + ",");
		ans.append(to_string(index) + ",");
		ans.append(to_string(left) + ",");
		for(int i=0; i<(int)right.elements.size(); i++)
		{
			ans.append(to_string(right.elements[i].type) + "/" + to_string(right.elements[i].value));
			ans.append("|");
		}
		if(right.elements.size() > 0)
			ans.erase(ans.end() - 1);
		ans.append(":");
		ans.append(to_string(right.index));
		return ans;
	}

	void deserialize(string data)
	{
		vector<string> splits = splitString(data, ',');
		op = (operation_t)stoi(splits[0]);
		index = stoi(splits[1]);
		left = (noterminal_symbol_t)stoi(splits[2]);
		splits = splitString(splits[3], ':');
		if(splits.size() > 1)
		{
			right.index = stoi(splits[1]);
			splits = splitString(splits[0], '|');
			for(int i=0; i<(int)splits.size(); i++)
			{
				vector<string> val = splitString(splits[i], '/');
				right.elements.push_back(element_t(stoi(val[0]), stoi(val[1])));
			}
		}
	}
};

struct ClosureElement
{
	noterminal_symbol_t left;
	rightElement right;
	int split_pos;
	firstSet first;
	bool end;
	bool operator==(const ClosureElement& other)
	{
		if(other.left != left || split_pos != other.split_pos || !(right == other.right))
			return false;
		return true;
	}

	bool needConclusion()
	{
		return split_pos >= (int)right.elements.size();
	}
};

struct ClosureState
{
	vector<ClosureElement> elements;
	int index;
	map<int, ClosureState*> go;
	ClosureState* parent;
	element_t from_element;
	bool operator==(const ClosureState& other)
	{
		for(ClosureElement element: other.elements)
		{
			bool equal = false;
			for(ClosureElement mine: elements)
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
};

void generateLR1Table(string gramma_path, string save_path, LR1Element**& table, int& width, int& height);
void LoadLR1FromFile(string path, LR1Element**& table, int& width, int& height);
void printTableToFile(string path, LR1Element** table, int width, int height);

map<noterminal_symbol_t, grammaNode> getGrammaFromFile(string path);
rightElement parseRight(string right, bool& contains_varepsilon);
element_t parseString(string right);
void getLeftFirst(map<noterminal_symbol_t, grammaNode>& all_nodes);
firstSet getFirst(rightElement node, map<noterminal_symbol_t, grammaNode> all_nodes); // 获得node的first集
vector<followSet> getFollow(grammaNode node, vector<grammaNode> all_nodes);

void getLR1(map<noterminal_symbol_t, grammaNode> nodes, LR1Element**& table, int& width, int& height);
bool checkConflict(ClosureElement element, vector<ClosureElement>& elements);
bool saveTableToFile(string path, LR1Element** table, int width, int height);

bool stateEqual(ClosureState* i, ClosureState* other);

class LR1Table
{
public:
	LR1Table(){};
	void init(string file);
    void reset();
	bool parseToken(element_t token);
	void printTable(string path="build/print_table.csv");
private:
	LR1Element** table;
	int width;
	int height;
    stack<int> state;// 状态栈
    stack<element_t> element;// 符号栈
    int num;// 用来计算输出的编号
};

#endif