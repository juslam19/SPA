#pragma once

#include <string>

#include "sp/ast/grammar_nodes/TermNode.h"
/*
Covers var_name, parenthesis, and const_value
*/
class FactorNode : public TermNode {
public:
    FactorNode(std::string value, std::string type, int stmtNumber) : TermNode(value, type, stmtNumber) {}

    void accept(AstVisitor* visitor) override {
        visitor->visitFactor(this);
    }
};
