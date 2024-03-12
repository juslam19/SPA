#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "AbstractionExtractor.h"

class PatternExtractor : public AbstractionExtractor {
public:
    PatternExtractor() {}
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

    std::unordered_set<std::pair<StmtNum, std::pair<std::string, std::string>>> getPattern();
    std::unordered_map<StmtNum, std::pair<std::string, std::shared_ptr<Matchable>>> getMatchablePattern();
};
