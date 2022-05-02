#ifndef ENUMREFLECT_H
#define ENUMREFLECT_H
#include <string>
#include<unordered_map>
#include <sstream>
using namespace std;

// Search and remove whitespace from both ends of the string
static std::string TrimEnumString(const std::string &s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && isspace(*it)) { it++; }
    std::string::const_reverse_iterator rit = s.rbegin();
    if(s.find('=') != s.npos)//排除等号和后面的干扰
    {
        bool meet_equal = false;
        while(rit.base() != it && (isspace(*rit) || !meet_equal))
        {
            if(*rit == '=')
            {
                meet_equal = true;
            }
            rit++;
        }
    }
    else
    {
        while (rit.base() != it && isspace(*rit)) { rit++; }
    }
    return std::string(it, rit.base());
}

static void SplitEnumArgs(const char* szArgs, std::unordered_map<std::string, int>& Array, int nMax)
{
    std::stringstream ss(szArgs);
    std::string strSub;
    int nIdx = 0;
    while (ss.good() && (nIdx < nMax)) {
        getline(ss, strSub, ',');
        Array[TrimEnumString(strSub)] = nIdx;
        nIdx++;
    }
};
// This will to define an enum that is wrapped in a namespace of the same name along with ToString(), FromString(), and COUNT
#define MAKE_ENUM(ename, ...) \
    enum ename { __VA_ARGS__, ename##COUNT }; \
    static std::unordered_map<std::string, int> ename##_map; \
    static ename ename##_fromstring(const std::string& strEnum) { \
    if (ename##_map.empty()) { SplitEnumArgs(#__VA_ARGS__, ename##_map, ename##COUNT); } \
	std::unordered_map<string, int>::iterator iter;\
    if((iter=ename##_map.find(strEnum)) != ename##_map.end())return (ename)iter->second;\
    return ename##COUNT; \
    } \
	static string ename##_tostring(const ename name){\
		if (ename##_map.empty()) { SplitEnumArgs(#__VA_ARGS__, ename##_map, ename##COUNT); } \
		for(auto iter=ename##_map.begin(); iter!=ename##_map.end(); iter++){\
			if(iter->second == name)\
				return iter->first;\
		}\
		return "";\
	}\

#define DEFINE_STRING(class_name)\
    std::string class_name::_Strings[class_name::COUNT];

#endif // ENUM_HELPER_H
