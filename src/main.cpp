// #include<stdio.h>
// #include "GrammaAnalyse.h"
// #include "lexer.hpp"

// LR1Table table;
// Lexer lexer;

// void init()
// {
// 	lexer.init();
// 	table.init("build/table.txt");
// }

// int main(int argc, char** argv)
// {
// 	system("chcp 65001");
// 	init();
// //    cout<<SELECT<<root<<endl;
//     FILE* file = fopen("doc/test.txt", "r");
// 	if(!file)
// 	{
// 		cout << "file can't open" << endl;
// 		return -1;
// 	}
// 	while(!feof(file))
// 	{
// 		string s; // 词法分析结果
// 		char buffer[1024];
// 		fgets(buffer, 1024, file);
//         table.reset();
// 		lexer.run(buffer);
// 		while(lexer.next(true))
// 		{
// 			// 词法分析
// 			Token token = lexer.get_token();
// 			s.assign(token.type);
// 			//语法分析
// 			element_t ele = parseString(s);
// 			if(!table.parseToken(ele))
// 			{
// 				cout << "语法分析错误" << endl;
// 				return -2;
// 			}
// 		}
// 		element_t last_token;
// 		last_token.type = 0;
// 		last_token.terminal = VAREPSILON;
// 		if(!table.parseToken(last_token))
// 		{
// 			cout << "语法分析错误" << endl;
// 			return -2;
// 		}
// 		else
// 		{
// 			cout << "归约成功" << endl;
// 		}
// 	}
// 	fclose(file);
// 	return 0;
// }

