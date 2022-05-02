#include "utils.h"

std::vector<std::string> splitString(const std::string& str, 
           const char delim) {
    std::vector<std::string> tokens;
    
    std::istringstream iss(str);
    std::string tmp;
    while (std::getline(iss, tmp, delim)) {
        if (tmp != "") {
            // 如果两个分隔符相邻，则 tmp == ""，忽略。
            tokens.emplace_back(std::move(tmp));
        }
    }
	return tokens;
}