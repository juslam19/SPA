#pragma once
#include <string>

#include "sp/ast/grammar_nodes/FactorNode.h"
class ConstantNode : public FactorNode {
public:
    explicit ConstantNode(std::string value, int stmtNumber) : FactorNode(value, "const", stmtNumber), value(value) {}
    std::string value;
    void accept(AstVisitor* visitor) override {
        visitor->visitConstant(this);
    }
};
