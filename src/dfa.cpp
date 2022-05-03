#include "dfa.hpp"

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
        int k = is_any_unmarked(_dfa_state_table);
        _dfa_state_table[k].marked = true;

        for(auto w = _alphabet.begin(); w != _alphabet.end() - 1; w++) {

        }
    }
}

void NFAToDFA::read_file(std::string filename) {
    std::string line;
    // std::ifstream myfile(filename);
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

        //READ IN THE ALPHABET
        std::getline(myfile, line);
        std::string trash;
        char move;
        std::istringstream alphabet = std::istringstream(line);

        //consume the "state" word
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