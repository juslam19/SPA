#include "IfNode.h"

void IfNode::accept(AstVisitor* visitor) {
    visitor->visitIf(this);
}

std::unique_ptr<ExpressionNode> IfNode::getCond() {
    std::vector<std::unique_ptr<ASTNode>> _children = this->getChildren();
    auto exprNodePtr = dynamic_cast<ExpressionNode*>(_children.at(0).get());
    std::unique_ptr<ExpressionNode> exprUniquePtr = std::make_unique<ExpressionNode>(*exprNodePtr);
    return exprUniquePtr;
}

std::unique_ptr<StatementListNode> IfNode::getThenStmtLstNode() {
    std::vector<std::unique_ptr<ASTNode>> _children = this->getChildren();
    auto stmtLstNodePtr = dynamic_cast<StatementListNode*>(_children.at(1).get());
    std::unique_ptr<StatementListNode> stmtLstUniquePtr = std::make_unique<StatementListNode>(*stmtLstNodePtr);
    return stmtLstUniquePtr;
}

std::unique_ptr<StatementListNode> IfNode::getElseStmtLstNode() {
    std::vector<std::unique_ptr<ASTNode>> _children = this->getChildren();
    auto stmtLstNodePtr = dynamic_cast<StatementListNode*>(_children.at(2).get());
    std::unique_ptr<StatementListNode> stmtLstUniquePtr = std::make_unique<StatementListNode>(*stmtLstNodePtr);
    return stmtLstUniquePtr;
}
