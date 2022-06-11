#include<stdio.h>
#include <string>
// #include "GrammaAnalyse.h"
#include "lexer.hpp"
#include "dfa.hpp"

// LR1Table table;
// Lexer lexer;

// void init()
// {
// 	lexer.init();
// 	table.init("build/table.txt");
// }


int main(int argc, char** argv)
{
    // FILE* file = fopen("examples/test0B.txt", "r");
	// if(!file)
	// {
	// 	cout << "file can't open" << endl;
	// 	return -1;
	// }
	// while(!feof(file))
	// {
	// 	std::string s; // 词法分析结果
	// 	char buffer[1024];
	// 	fgets(buffer, 1024, file);
    //     // table.reset();
	// 	lexer.run(buffer);
	// 	while(lexer.next(true))
	// 	{
	// 		// 词法分析
	// 		Token token = lexer.get_token();
	// 		s.assign(token.type);
	// 		//语法分析
	// 		// element_t ele = parseString(s);
	// 		// if(!table.parseToken(ele))
	// 		// {
	// 		// 	cout << "语法分析错误" << endl;
	// 		// 	return -2;
	// 		// }
	// 	}
	// // 	element_t last_token;
	// // 	last_token.type = 0;
	// // 	last_token.terminal = VAREPSILON;
	// // 	if(!table.parseToken(last_token))
	// // 	{
	// // 		cout << "语法分析错误" << endl;
	// // 		return -2;
	// // 	}
	// // 	else
	// // 	{
	// // 		cout << "归约成功" << endl;
	// // 	}
	// }
	// fclose(file);
	// return 0;
#ifdef LEXER
    // 对词法分析器进行初始化
	Lexer lexer;
    lexer.init();
	if(argc < 2){
		return -1;
	}
    FILE* file = fopen(argv[1], "r");
    if(file){
        char* buf = new char[512];
        fread(buf, 1, 512, file);
        lexer.run(buf);
		while(lexer.next(true));
    }
#endif

#ifdef DFA
		NFAToDFA nfa_to_dfa = NFAToDFA("examples/dfa_test.txt");
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
		return 0;
#endif
}

