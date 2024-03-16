#include "sp/ast/SemanticValidator.h"

#include <memory>
#include <unordered_map>
#include <utility>

#include "sp/ast/SemanticValidatorVisitor.h"
#include "sp/de/AstVisitor.h"
#include "sp/de/NodeDeclarations.h"
#include "sp/exceptions/semantic/DuplicateProcError.h"

void SemanticValidator::validateSemantics(const std::shared_ptr<ProgramNode> programNode) {
    procedureNames = std::make_shared<std::unordered_map<std::string, std::vector<std::string>>>();
    checkDuplicateProcedureNames(programNode);
    this->semanticValidatorVisitor = new SemanticValidatorVisitor(
        std::shared_ptr<std::unordered_map<std::string, std::vector<std::string>>>(procedureNames));
    checkCallingProcedure(programNode);
}

void SemanticValidator::checkDuplicateProcedureNames(std::shared_ptr<ProgramNode> programNode) {
    for (auto procedure : programNode->children) {
        std::string procedureName = procedure->name;
        if (procedureNames->find(procedureName) != procedureNames->end()) {
            throw DuplicateProcedureError(procedureName);
        }
        procedureNames->insert(std::make_pair(procedureName, std::vector<std::string>{}));
    }
}

void SemanticValidator::checkCallingProcedure(const std::shared_ptr<ProgramNode> programNode) {
    visitNode(programNode, this->semanticValidatorVisitor);
}

void SemanticValidator::visitNode(std::shared_ptr<ASTNode>&& node, SemanticValidatorVisitor* visitor) {
    if (!node) {
        return;
    }
    node->accept(visitor);

    // TODO(ben): casting pointer for every node is slow and impractical.
    // A better way would be to have an enum of the different names and then perform a check on the
    // value of the node instead.
    if (auto procedure = std::dynamic_pointer_cast<ProcedureNode>(node)) {
        currentProcedure = procedure->name;
    }

    for (auto child : node->getChildren()) {
        visitNode((std::shared_ptr<ASTNode>&&)child, visitor);
    }
}
