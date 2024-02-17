#include <string>

#include "AstNode.h"
class NameNode : public ASTNode {
public:
    explicit NameNode(std::string name) : ASTNode(name, "var") {}
};
