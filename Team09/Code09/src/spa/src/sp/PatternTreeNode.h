#include <memory>
#pragma once
#include <iostream>

#include "sp/ast/grammar_nodes/ExpressionNode.h"
#include "string"

// Some tree operations are adapted from the following solutions:
// Serialising and deserialising:
// https://leetcode.com/problems/serialize-and-deserialize-binary-tree/solutions/74259/recursive-preorder-python-and-c-o-n/
// Equality: https://leetcode.com/problems/same-tree/solutions/3746149/recursive-approach-with-easy-steps/
// Partial Match: https://leetcode.com/problems/subtree-of-another-tree/description/
class PatternTreeNode {
public:
    PatternTreeNode(std::string value) : value(value), left(nullptr), right(nullptr) {}
    PatternTreeNode();
    static std::shared_ptr<PatternTreeNode> buildTreeFromString(std::string value);
    static std::shared_ptr<PatternTreeNode> buildTreeFromAST(std::shared_ptr<ExpressionNode> node);
    static std::string serialiseToString(std::shared_ptr<PatternTreeNode> node);
    static std::shared_ptr<PatternTreeNode> deserializeToNode(std::string);

    void setLeft(std::shared_ptr<PatternTreeNode> left);
    void setRight(std::shared_ptr<PatternTreeNode> right);

    friend std::ostream& operator<<(std::ostream& os, const PatternTreeNode& obj) {
        if (obj.left == nullptr && obj.right == nullptr) {
            os << "{ \"value\": \"" << obj.value << "\" } ";
            return os;
        }
        os << "{ \"value\": \"" << obj.value << "\", "
           << "\"left\": {" << *(obj.left) << "}"
           << ", "
           << "\"right\": {" << *(obj.right) << "}"
           << "}";

        return os;
    }

    static bool isEqual(const std::shared_ptr<PatternTreeNode>& tree1, const std::shared_ptr<PatternTreeNode>& tree2);
    static bool isPartiallyEqual(const std::shared_ptr<PatternTreeNode>& tree1,
                                 const std::shared_ptr<PatternTreeNode>& tree2);

private:
    std::string value;
    std::shared_ptr<PatternTreeNode> left;
    std::shared_ptr<PatternTreeNode> right;
    static void serializeHelper(std::shared_ptr<PatternTreeNode> node, std::ostringstream& out);
    static std::shared_ptr<PatternTreeNode> deserializeHelper(std::istringstream& in);
};
