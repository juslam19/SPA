#pragma once

#include <regex>

struct QPSRegexes {
    // Select{>=1 whitespaces}{capturing group}
    inline static const std::regex SELECT_CLAUSE = std::regex("\\s*Select\\s+(\\w+)\\s*");
    // {>=1 whitespaces}such{>=1 whitespaces}that{>=1 whitespaces}{capturing group}
    // capturing group format -> {letters/digits}{optional *}{>=0 whitespaces}{bracketed non-greedy}
    inline static const std::regex SUCHTHAT_CLAUSE = std::regex("\\s+such\\s+that\\s+(\\w+\\*?\\s*\\(.*?\\))\\s*");
    // Pattern Clause
    inline static const std::regex PATTERN_CLAUSE = std::regex("\\s+pattern\\s+(\\w+\\s*\\(.*?\\))\\s*");
    
    // <{letters/digits}{optional *}>{>=0 whitespaces}<{bracketed non-greedy}>
    inline static const std::regex SUCHTHAT_ARGS = std::regex("\\s*(\\w+\\*?)\\s*\\((.*?)\\)\\s*");
    // <{letters/digits}>{>=0 whitespaces}<{bracketed non-greedy}>
    inline static const std::regex PATTERN_ARGS = std::regex("\\s*(\\w+)\\s*\\((.*?)\\)\\s*");
};
