#pragma once
#include <string>

#include "sp/ast/AstNode.h"
#include "sp/ast/grammar_nodes/FactorNode.h"
class ConstantNode : public FactorNode {
public:
    explicit ConstantNode(std::string value) : FactorNode(value, "const") {}

    void accept(AstVisitor* visitor) override {
        visitor->visitConstant(this);
    }
};
