#include "dfa.hpp"

void NFAToDFA::print_vec(std::vector<int> vec) {
    std::cout << "{";
    for(std::vector<int>::const_iterator i = vec.begin(); i != vec.end(); i++) {
        if(i != vec.end() - 1){
            std::cout<< *i <<",";
        }else {
            std::cout<< *i;
        }
    }
    std::cout<< "}";
    std::cout<< "\n";
}

void NFAToDFA::print_vec_vec(std::vector<std::vector<int>> vec) {
    for(size_t i = 0; i < vec.size(); i++) {
        std::vector<int> child = vec[i];
        std::cout<<i << ": ";
        for(size_t j = 0; j < child.size(); j++) {
            std::cout<< child[j] << " ";
        }
        std::cout<<"\n";
    }
    std::cout<<"\n";
}

void NFAToDFA::erase_vec_vec(std::vector<std::vector<int>> &vec, std::vector<int> key) {
    for(auto i = vec.begin(); i != vec.end(); i++) {
        if(*i == key) {
            vec.erase(i);
            return;
        }
    }
}

DFAState NFAToDFA::new_dfa_state(bool mark, std::vector<int> s) {
    DFAState new_state;
    std::map<char, int> init;
    new_state.marked = mark;
    new_state.states = s;
    new_state.moves = init;
    return new_state;
}

bool NFAToDFA::vector_contain(std::vector<int> vec, int key) {
    for(auto &i: vec) {
        if(i == key) return true;
    }
    return false;
}

int NFAToDFA::find_key_in_vec(std::vector<std::vector<int>> vec, int key) {
    for(size_t i = 0; i < vec.size(); i++) {
        std::vector<int> child_vec = vec[i];
        if(std::find(child_vec.begin(), child_vec.end(), key) != child_vec.end()) {
            return (int)i;
        }
    }
    return -1;
}

int NFAToDFA::is_any_unmarked(DFATable dfa_table) {
    for(long unsigned int i = 0; i < dfa_table.size(); i++) {
        if(!dfa_table[i].marked)return i;
    }
    return -1;
}

int NFAToDFA::dfa_state_contain(std::vector<int> state, DFATable dfa_table) {
    for(long unsigned int i = 0; i < dfa_table.size(); i++) {
        if(state == dfa_table[i].states) return i;
    }
    return -1;
}


// 根据闭包获取的对应所有的元素
std::vector<int> NFAToDFA::eclosure(std::vector<int> T, NFATable nfa_table) {
    std::vector<int> eclosure;
    std::stack<int> stack;

    // 将该状态内元素 push 到栈里面
    for(std::vector<int>::const_iterator j = T.begin(); j != T.end(); j++) {
        stack.push(*j);
    }

    // 初始化闭包
    for(std::vector<int>::const_iterator i = T.begin(); i != T.end(); i++) {
        eclosure.push_back(*i);
    }

    while(!stack.empty()) {
        int cur = stack.top();
        stack.pop();

        // 通过该元素能够通过闭包到达的所有元素
        std::map<char, std::vector<int>> current_state = nfa_table[cur];
        // E 表示 epsilon
        std::vector<int> EMoves = current_state['E'];

        for(std::vector<int>::const_iterator i = EMoves.begin(); i != EMoves.end(); i++) {
            // 如果元素没有在里面，将其添加到里面
            if(!vector_contain(eclosure, *i)) {
                eclosure.push_back(*i);
                stack.push(*i);
            }
        }
    }
    std::sort(eclosure.begin(), eclosure.end());
    return eclosure;
}

// 根据一次元素 move 获取对应所有元素组成的状态
std::vector<int> NFAToDFA::move(std::vector<int> T, char ele, NFATable nfa_table){
    std::vector<int> res;
    // 遍历给定的状态，找到可以通过 ele 发现的所有元素
    for(auto &i: T) {
        auto reachable_states = nfa_table[i][ele];
        for(auto &j: reachable_states) {
            if(!vector_contain(res, j)) {
                res.push_back(j);
            }
        }
    }
    return res;
}

std::vector<int> NFAToDFA::find_final_dfa() {
    std::vector<int> finals;
    for(unsigned long int i = 0; i < _dfa_state_table.size(); i++){
        for(std::vector<int>::const_iterator k = _final_states.begin(); k != _final_states.end(); k++){
            if( vector_contain(_dfa_state_table[i].states, *k) ){
                finals.push_back(i);
            }
        }
    }
    this->_dfa_final_states = finals;
    return finals;
}

// DFA 确定化
void NFAToDFA::nfa_determine() {
    int current_dfa_state_num = 0;

    std::vector<int> init_state_vec;
    init_state_vec.push_back(_init_state);

    std::vector<int> eclos = eclosure(init_state_vec, _nfa_state_table);

    // 新建 DFA 状态为空
    DFAState init_state = new_dfa_state(false, eclos);
    _dfa_state_table[current_dfa_state_num] = init_state;
    current_dfa_state_num++;

    while(is_any_unmarked(_dfa_state_table) >= 0) {
        // 找到第一个没有被标记的 DFA state
        int k = is_any_unmarked(_dfa_state_table);
        // 将该 state 设置为 marked
        _dfa_state_table[k].marked = true;

        // 遍历所有字母找到下一步状态
        for(auto w = _alphabet.begin(); w != _alphabet.end() - 1; w++) {
            // 获取某元素 move 后的状态
            std::vector<int> theMove = move(_dfa_state_table[k].states, *w, _nfa_state_table);
            // 获取若干次 epilision 后的状态
            std::vector<int> alphaMove = eclosure(theMove, _nfa_state_table);
            // 查看状态是否重复
            int j = dfa_state_contain(alphaMove, _dfa_state_table);
            if(j >= 0) {
                _dfa_state_table[k].moves[*w] = j;
            }else {
                if(!alphaMove.empty()) {
                    DFAState new_state = new_dfa_state(false, alphaMove);
                    _dfa_state_table[current_dfa_state_num] = new_state;
                    _dfa_state_table[k].moves[*w] = current_dfa_state_num;
                    current_dfa_state_num++;
                }else {
                    _dfa_state_table[k].moves[*w] = -1;
                }
            }
        }
    }
}

// DFA 最小化
std::vector<std::vector<int>> NFAToDFA::dfa_minialize() {
    // 首先将 DFA Table 分为终结状态和非终结状态
    std::queue<std::vector<int>> dfa_partiation;
    std::vector<std::vector<int>> dfa_min_ans;
    // 非终结状态
    std::vector<int> non_finial_state;
    for(size_t i = 0; i < _dfa_state_table.size(); i++) {
        if(std::find(_dfa_final_states.begin(), _dfa_final_states.end(), i) == _dfa_final_states.end()) {
            non_finial_state.push_back(i);
        }   
    }

    dfa_partiation.push(non_finial_state);
    dfa_partiation.push(_dfa_final_states);
    dfa_min_ans.push_back(non_finial_state);
    dfa_min_ans.push_back(_dfa_final_states);

    // 分别读入不同的符号来进行下一步划分
    // 主要查看该状态在 move 后是否会变为其他状态
    for(auto &ele: _alphabet) {
        // 遍历所有字母
        // 查看某个子集所有元素 move 后仍属一个集合
        while(!dfa_partiation.empty()) {
            // 从队列中取出一个集合
            auto set = dfa_partiation.front();
            dfa_partiation.pop();
            std::vector<int> set_a;
            std::vector<int> set_b;
            std::optional<int> a;
            std::optional<int> b;
            for(auto &state: set) {
                // 遍历每个子集的状态
                // 获取到 move 后的状态
                int move_state = _dfa_state_table[state].moves[ele];
                int move_set = find_key_in_vec(dfa_min_ans, move_state);
                if(!a.has_value()) {
                    a.emplace(move_set);
                    set_a.push_back(state);
                }else{
                    if(move_set == a.value()) {
                        set_a.push_back(state);
                    }else {
                        if(!b.has_value()) {
                            b.emplace(move_set);
                            set_b.push_back(state);
                        }else{
                            set_b.push_back(state);
                        }
                    }
                }
            }
            if(!b.has_value()) {
                // 如果只有一个集合的话不动
            }else{
                // dfa_min_ans.erase(dfa_min_ans.begin());
                erase_vec_vec(dfa_min_ans, set);
                dfa_partiation.push(set_a);
                dfa_partiation.push(set_b);
                dfa_min_ans.push_back(set_a);
                dfa_min_ans.push_back(set_b);
            }
        }
        // 将所有得到的序列 push 进队列进行下一次 move
        for(auto &i: dfa_min_ans) {
            dfa_partiation.push(i);
        }
    }
    return dfa_min_ans;
}

void NFAToDFA::print_dfa_final_state() {
    std::cout<< "Final State: \n";
    std::vector<int> final_states = find_final_dfa();
    print_vec(final_states);
    std::cout<< "\n";
    for(size_t i = 0; i < final_states.size(); i++) {
        std::cout<< "State " << final_states[i] << ":";
        print_vec(_dfa_state_table[final_states[i]].states);
        std::cout<< "\n";
    }
    std::cout<<"\n";
}

void NFAToDFA::print_dfa_table() {
    std::cout << "State      ";
    for(std::vector<char>::const_iterator k = _alphabet.begin(); k != _alphabet.end() - 1; k++){
        std::cout << *k << "        ";
    }
    std::cout << std::endl;

    for(size_t i = 0; i < _dfa_state_table.size(); i++){
        // for each DFA State, print out the moves for each alphabet symbol.
        std::cout << i << "         ";
        for(std::vector<char>::const_iterator k = _alphabet.begin(); k != _alphabet.end() - 1; k++){
        std::cout << OPENING_BRACKET;
        if(_dfa_state_table[i].moves[*k] != -1){
            std::cout << _dfa_state_table[i].moves[*k];
        }
        std::cout << CLOSING_BRACKET << "       ";
        }
        std::cout << std::endl;
    }

    for(size_t i = 0; i < _dfa_state_table.size(); i++){
        std::cout<< "State "<< i<< " {";
        for(auto &ele: _dfa_state_table[i].states) {
            std::cout<< ele<< " ";
        }
        std::cout<<"}\n";
    }
}

void NFAToDFA::read_file(std::string filename) {
    std::string line;
    std::ifstream myfile = std::ifstream(filename);

    if(myfile.is_open()) {

        std::getline(myfile, line);

        /*************************************
        * GET INITIAL STATE
        *************************************/
        std::istringstream iss(line);
        [[maybe_unused]] char test = iss.get();

        while(iss.peek() != OPENING_BRACKET){
            iss.ignore();
        }

        iss.ignore();
        iss >> _init_state;

        /*************************************
        * GET FINAL STATES
        *************************************/
        std::getline(myfile, line);
        iss.str(line);

        while(iss.peek() != OPENING_BRACKET){
        iss.ignore();
        }

        iss.ignore();
        int finalState;
        iss >> finalState;
        _final_states.push_back(finalState);

        //could have multiple final states. Look for a , loop until we hit the closing brace.
        while(iss.peek() != CLOSING_BRACKET){
        if(iss.peek() == ','){
            iss.ignore();
        }
        iss >> finalState;
        _final_states.push_back(finalState);
        }

        /*************************************
        * GET TOTAL STATES
        *************************************/
        std::getline(myfile, line);
        iss.str(line);

        while(iss.peek() != ':'){
        iss.ignore();
        }
        iss.ignore();
        iss.ignore();
        iss >> _total_states;

        /*************************************
        * INITIALIZE NFA STATES
        *************************************/

        // 获取字母
        std::getline(myfile, line);
        std::string trash;
        char move;
        std::istringstream alphabet = std::istringstream(line);

        // 将 "state" 消耗掉
        alphabet >> trash;

        while(alphabet >> move){
            _alphabet.push_back(move);
        }

        std::getline(myfile, line);

        for(int i = 1; i <= _total_states; i++){
            std::istringstream _iss(line);
            std::map<char, std::vector<int> > StateMovesMap;

            for(unsigned long int j = 0; j < _alphabet.size(); j++) {
                int state;
                std::string bracketMoves;
                std::vector<int> states;

                /*
                * We are on a fresh line.ignore the state integer and the
                * blank space.
                */
                if(j == 0){
                _iss.ignore();
                _iss.ignore();
                }
                else{
                /*
                * We are parsing moves after a, so just need to ignore the whitespace.
                */
                _iss.ignore();
                }

                _iss >> bracketMoves;
                //remove the opening and closing bracket
                bracketMoves = bracketMoves.substr(1,bracketMoves.size()-2);

                if(bracketMoves != ""){

                    std::istringstream movestr = std::istringstream(bracketMoves);

                    while(movestr >> state){
                        states.push_back(state);
                        if(movestr.peek() == ','){
                            movestr.ignore();
                        }
                    }
                }

                /*
                * Finished parsing states for a move.
                * Insert it into the StateMovesMap
                */
                std::sort(states.begin(), states.end());
                StateMovesMap[ _alphabet[j] ] = states;
            }//End Moves Loop

            /*
            * Finishing parsing all moves for a state. Insert the StateMovesMap
            * into the STATE_TABLE, which will be used in the subset and e-closure
            * functions
            */
            _nfa_state_table[i] = StateMovesMap;
            std::getline(myfile, line);
        } //End Total States Loop
    } //End If My File is_open();
}