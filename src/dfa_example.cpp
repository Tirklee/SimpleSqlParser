#include "dfa.hpp"

int main() {
	NFAToDFA nfa_to_dfa = NFAToDFA("examples/dfa_test2.txt");
	nfa_to_dfa.nfa_determine();
	nfa_to_dfa.print_dfa_final_state();
    std::cout<<"DFA Determinate: \n";
    nfa_to_dfa.print_dfa_table();
    std::cout<< "\n";
    std::vector<std::vector<int>> dfa = nfa_to_dfa.dfa_minialize();
    std::cout<< "DFA Minialize: \n";
    for(size_t i = 0; i < dfa.size(); i++) {
        std::vector<int> child = dfa[i];
        std::cout<<"State "<<i << ": ";
        std::cout<< "{";
        for(size_t j = 0; j < child.size(); j++) {
            std::cout<< child[j] << " ";
        }
        std::cout<<"}";
        std::cout<<"\n";
    }
    std::cout<<"\n";
}