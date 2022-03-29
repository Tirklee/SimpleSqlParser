#ifndef TOKEN_HPP_
#define TOKEN_HPP_

#include <string>

enum Tokens {
    Int, Float, Idn, Str,
    Eq, Ne, Lt, Le, Gt, Ge,
    And, Or, Xor   
};

struct Identifiers
{
    Tokens token;
    int hash;
    std::string name;
    
};


enum Errors {

};
#endif