#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "sp/ast/AstNode.h"
#include "sp/tokenizer/Token.h"
class ProgramNode : public ASTNode {
public:
    explicit ProgramNode(std::vector<std::shared_ptr<ASTNode>> children)
        : ASTNode("main", "program", (children), -1) {
    }

    void accept(AstVisitor* visitor) override {
        visitor->visitProgram(this);
    }
};
