#include "ExpressionNode.h"

#include "ConstantNode.h"
#include "VariableNode.h"

void ExpressionNode::accept(AstVisitor* visitor) {
    visitor->visitExpression(this);
}

std::vector<std::string> ExpressionNode::getVars() {
    if (variables.size() == 0) {
        findVariables(this->getChildren());
    }
    return variables;
}

std::vector<std::string> ExpressionNode::getConsts() {
    if (constants.size() == 0) {
        findConstants(this->getChildren());
    }
    return constants;
}

// Function to traverse through all children nodes and retrieve variable nodes
void ExpressionNode::findVariables(const std::vector<std::shared_ptr<ASTNode>>& children) {
    if (children.size() == 0) {
        if (auto variableNodePtr = dynamic_cast<VariableNode*>(this)) {
            variables.push_back(variableNodePtr->value);
        }
    }
    for (auto& child : children) {
        // Use dynamic_cast to check if the child node is of type VariableNode
        if (auto variableNodePtr = dynamic_cast<VariableNode*>(child.get())) {
            variables.push_back(variableNodePtr->value);
        }
        // Recursively traverse child nodes
        findVariables(child->getChildren());
    }
}

// Function to traverse through all children nodes and retrieve constant nodes
void ExpressionNode::findConstants(const std::vector<std::shared_ptr<ASTNode>>& children) {
    if (children.size() == 0) {
        if (auto constantNodePtr = dynamic_cast<ConstantNode*>(this)) {
            constants.push_back(constantNodePtr->value);
        }
    }
    for (auto& child : children) {
        // Use dynamic_cast to check if the child node is of type ConstantNode
        if (auto constantNodePtr = dynamic_cast<ConstantNode*>(child.get())) {
            constants.push_back(constantNodePtr->value);
        }
        // Recursively traverse child nodes
        findConstants(child->getChildren());
    }
}
