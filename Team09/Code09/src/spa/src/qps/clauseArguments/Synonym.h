#pragma once

#include <string>

#include "ClauseArgument.h"

enum class DesignEntityType {
    STMT,
    READ,
    CALL,
    WHILE,
    IF,
    ASSIGN,
    VARIABLE,
    CONSTANT,
    PROCEDURE,
};

class Synonym : public ClauseArgument {
private:
    DesignEntityType type;
    std::string name;

public:
    Synonym(const DesignEntityType& t, const std::string& n);

    static DesignEntityType determineType(const std::string);

    DesignEntityType getType() const;
    std::string getName() const;
    static std::string entityTypeToString(DesignEntityType);
    void print();
    bool operator==(const Synonym& other) const;
};
