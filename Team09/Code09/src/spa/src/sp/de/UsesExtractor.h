#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "AbstractionExtractor.h"
#include "NodeDeclarations.h"
#include "ProcedureTracker.h"

class UsesExtractor : public AbstractionExtractor {
public:
    std::unordered_map<std::string, ProcedureNode*> procs;
    std::unordered_map<std::string, std::unordered_set<std::pair<StmtNum, Variable>>>* extractedProcs;
    std::unordered_map<WhileNode*, std::unordered_set<std::pair<StmtNum, Variable>>>* extractedWhiles;
    std::unordered_map<IfNode*, std::unordered_set<std::pair<StmtNum, Variable>>>* extractedIfs;

    std::string currentProc;

    explicit UsesExtractor(
        std::unordered_map<std::string, ProcedureNode*> procs,
        std::unordered_map<std::string, std::unordered_set<std::pair<StmtNum, Variable>>>* extractedProcs,
        std::unordered_map<WhileNode*, std::unordered_set<std::pair<StmtNum, Variable>>>* extractedWhiles,
        std::unordered_map<IfNode*, std::unordered_set<std::pair<StmtNum, Variable>>>* extractedIfs)

        : procs(procs), extractedProcs(extractedProcs), extractedWhiles(extractedWhiles), extractedIfs(extractedIfs) {}

    void visitStmtLst(StatementListNode* node) override;
    void visitProgram(ProgramNode* node) override;
    void visitProcedure(ProcedureNode* node) override;
    void visitRead(ReadNode* node) override;
    void visitPrint(PrintNode* node) override;
    void visitWhile(WhileNode* node) override;
    void visitIf(IfNode* node) override;
    void visitAssign(AssignmentNode* node) override;
    void visitExpression(ExpressionNode* node) override;
    void visitFactor(FactorNode* node) override;
    void visitTerm(TermNode* node) override;
    void visitVariable(VariableNode* node) override;
    void visitConstant(ConstantNode* node) override;
    void visitCall(CallNode* node) override;

    std::unordered_set<std::pair<StmtNum, Variable>> getUses();
    std::unordered_set<std::pair<Procedure, Variable>> getProcedureUses();
    void dfsVisitHelper(std::shared_ptr<ASTNode> node, UsesExtractor* visitor);
    void dfsVisitHelper(ASTNode* node, UsesExtractor* visitor);
};
