#pragma once

#include <vector>
#include <string>
#include <unordered_set>

enum BASIC_TOKEN_TYPE {
    _NAME,
    _INTEGER,
    SYMBOL,
    KEYWORD
};

const std::unordered_set<std::string> KEYWORDS = {
    "procedure",
    "while",
    "if",
    "then",
    "else",
    "read",
    "print"
};

/**
 * @brief Provides a intermediate representation of each string to disambiguate between strings that have the same value but different type
 * such as the keyword 'procedure' and the var_name 'procedure'. 
 */
class BasicToken {        
    public:
        BASIC_TOKEN_TYPE type;
        std::string value;
        BasicToken(std::string value, BASIC_TOKEN_TYPE type);
};
