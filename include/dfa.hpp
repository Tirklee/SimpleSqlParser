#ifndef DFA_HPP__
#define DFA_HPP__

#include <algorithm>
#include <vector>
#include <stack>
#include <map>
#include <string>
#include <iostream>

const char OPENING_BRACKET = '{';
const char CLOSING_BRACKET = '}';

struct DFAState {
    bool marked;
    std::vector<int> states;
    std::map<char, int> moves;
};

// DFA 状态表
typedef std::map<int, DFAState> DFATable;
// NFA 状态表
typedef std::map<int, std::map<char, std::vector<int>>> NFATable;

class NFAToDFA {
    protected:
        int _init_state;
        int _total_states;
        std::vector<int> _final_states;
        std::vector<char> _alphabet;
        NFATable _state_table;
        DFAState _dfa_state_table;

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
    public:
        // 读取文件
        void read_file(std::string filename);
        // NFA 确定化
        void nfa_determine();
};

#endif