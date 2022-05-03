#ifndef DFA_HPP__
#define DFA_HPP__

#include <algorithm>
#include <vector>
#include <stack>
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>


const char OPENING_BRACKET = '{';
const char CLOSING_BRACKET = '}';

struct DFAState {
    // 是否被遍历过
    bool marked;
    // 当前的状态，使用字符集来表示
    std::vector<int> states;
    // 经过某字符的 move 后的下一状态的标号
    std::map<char, int> moves;
};

// DFA 状态表
typedef std::map<int, DFAState> DFATable;
// NFA 状态表
typedef std::map<int, std::map<char, std::vector<int>>> NFATable;

class NFAToDFA {
    protected:
        // 初始状态
        int _init_state;
        // 全部状态数
        int _total_states;
        // 最终状态
        std::vector<int> _final_states;
        // 字符表
        std::vector<char> _alphabet;
        // NFA 状态表
        NFATable _nfa_state_table;
        // DFA 状态表
        DFATable _dfa_state_table;
        // DFA 终态
        std::vector<int> _dfa_final_states;

        void print_vec(std::vector<int> vec);
        // 初始化 DFA state
        DFAState new_dfa_state(bool mark, std::vector<int> s);
        // 是否包含某个 key
        bool vector_contain(std::vector<int> vec, int key);
        // 查看是否有没有被标记的状态
        int is_any_unmarked(DFATable dfa_table);
        // 查看 DFA Table 中是否有该状态
        int dfa_state_contain(std::vector<int> state, DFATable dfa_table);
        // 根据给定的状态，返回 epsilon 闭包
        std::vector<int> eclosure(std::vector<int> T, NFATable nfa_table);
        // 根据给定的状态，返回根据元素移动后的状态
        std::vector<int> move(std::vector<int> T, char ele, NFATable nfa_table);
        // 查找最终的 DFA
        std::vector<int> find_final_dfa();
    public:
        // 读取文件
        void read_file(std::string filename);
        // NFA 确定化
        void nfa_determine();
        // DFA 最小化
        void dfa_minialize(DFATable& dfa_table, NFATable& nfa_table);
        // 打印 DFA 终态
        void print_dfa_final_state(){
            std::vector<int> final_states = find_final_dfa();
            print_vec(final_states);
            std::cout<< "\n";
            for(size_t i = 0; i < final_states.size(); i++) {
                std::cout<< "State " << final_states[i] << ":";
                print_vec(_dfa_state_table[final_states[i]].states);
                std::cout<< "\n";
            }
        }
        // 构造函数
        NFAToDFA(std::string filename){ read_file(filename); }
        // 析构函数
        ~NFAToDFA(){}
};

#endif