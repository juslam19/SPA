#include "PrintNode.h"

void PrintNode::accept(AstVisitor* visitor) {
    visitor->visitPrint(this);
}

std::string PrintNode::getVar() {
    std::vector<std::unique_ptr<ASTNode>> _children = this->getChildren();
    return _children.at(0)->getValue();
}
