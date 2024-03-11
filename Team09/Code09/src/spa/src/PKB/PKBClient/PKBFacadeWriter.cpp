#include "PKBFacadeWriter.h"

#include "PKB/PKB.h"

PKBFacadeWriter::PKBFacadeWriter(PKB &pkbReference) {
    this->pkbReference = &pkbReference;
}

void PKBFacadeWriter::setVariables(const std::unordered_set<Variable> &inputVariables) const {
    this->pkbReference->variableStore->addEntities(inputVariables);
}

void PKBFacadeWriter::setConstants(const std::unordered_set<Constant> &inputConstants) const {
    this->pkbReference->constantStore->addEntities(inputConstants);
}

void PKBFacadeWriter::setProcedures(const std::unordered_set<Procedure> &inputProcedures) const {
    this->pkbReference->procedureStore->addEntities(inputProcedures);
}

void PKBFacadeWriter::setStmts(const std::unordered_set<Stmt> &inputStmts) const {
    this->pkbReference->stmtStore->setStatements(inputStmts);
}

void PKBFacadeWriter::setFollowsStore(const std::unordered_set<std::pair<StmtNum, StmtNum>> &followsPairs) const {
    this->pkbReference->followsStore->setFollowsStore(followsPairs);
}

void PKBFacadeWriter::setParentStore(const std::unordered_set<std::pair<StmtNum, StmtNum>> &parentPairs) const {
    this->pkbReference->parentStore->setParentStore(parentPairs);
}

void PKBFacadeWriter::setUsesStore(const std::unordered_set<std::pair<StmtNum, Variable>> &usesPairs) const {
    this->pkbReference->usesStore->setUsesStore(usesPairs);
}

void PKBFacadeWriter::setStatementModifiesStore(
    const std::unordered_set<std::pair<StmtNum, Variable>> &modifiesPairs) const {
    this->pkbReference->modifiesStore->setStatementModifiesStore(modifiesPairs);
}

void PKBFacadeWriter::setPatternStore(
    const std::unordered_set<std::pair<StmtNum, std::pair<std::string, std::string>>> &patterns) const {
    this->pkbReference->patternStore->setPatterns(patterns);
}

void PKBFacadeWriter::setNextStore(const std::unordered_set<std::pair<StmtNum, StmtNum>> &nextPairs) const {
    this->pkbReference->nextStore->setNextStore(nextPairs);
}
