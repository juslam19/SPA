#include <string>
#include <vector>

#include "AstNode.h"
#include "sp/tokenizer/Token.h"
class ProgramNode : public ASTNode {
    ProgramNode() : ASTNode("main", "program") {}
};
