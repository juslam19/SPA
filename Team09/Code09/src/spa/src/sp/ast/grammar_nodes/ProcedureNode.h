#pragma once

#include <string>

#include "AstNode.h"

class ProcedureNode : public ASTNode {
public:
    explicit ProcedureNode(std::string name) : ASTNode(name, "proc") {}
};
