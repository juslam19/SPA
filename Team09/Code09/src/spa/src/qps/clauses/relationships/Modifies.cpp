#include "Modifies.h"

Modifies::Modifies(ClauseArgument& stmt, ClauseArgument& var) : stmt(stmt), var(var) {}

void Modifies::checkSemantic() {
    if (this->stmt.isWildcard()) {
        throw QPSSemanticError();
    }
    if (this->stmt.isSynonym()) {
        Synonym first = dynamic_cast<Synonym&>(this->stmt);
        if (first.getType() == DesignEntityType::VARIABLE || first.getType() == DesignEntityType::CONSTANT) {
            throw QPSSemanticError();
        }
    }
    if (var.isSynonym()) {
        Synonym second = dynamic_cast<Synonym&>(var);
        if (second.getType() != DesignEntityType::VARIABLE) {
            throw QPSSemanticError();
        }
    }
}

ClauseResult Modifies::evaluate(PKBFacadeReader& reader) {
    if (isSimpleResult()) {
        return {reader.hasStatementVariableModifiesRelationship(stmt, var)};
    }

    // Synonym, Literal
    if (stmt.isSynonym() && var.isLiteral()) {
        return statementsModifyLiteral(reader);
    }

    // Synonym, Wildcard
    if (stmt.isSynonym() && var.isWildcard()) {
        return allStmtsThatModifySomeVar(reader);
    }

    // Integer, Synonym
    if (stmt.isInteger() && var.isSynonym()) {
        return variablesModifedByStatement(reader);
    }

    // Wildcard, Synonym
    if (stmt.isWildcard() && var.isSynonym()) {
        return allModifiedVariables(reader);
    }

    // Synonym, Synonym
    return evaluateBothSynonyms(reader);
}

ClauseResult Modifies::evaluateBothSynonyms(PKBFacadeReader& reader) {
    Synonym stmtSyn = static_cast<Synonym&>(stmt);
    Synonym varSyn = static_cast<Synonym&>(var);

    SynonymValues stmtValues{};
    SynonymValues varValues{};

    for (Variable var : reader.getVariables()) {
        std::unordered_set<StmtNum> stmts = reader.getModifiesStatementsByVariable(var);
        for (StmtNum currStmt : filterStatementsByType(reader, stmtSyn.getType(), stmts)) {
            stmtValues.push_back(std::to_string(currStmt));
            varValues.push_back(var);
        }
    }

    std::vector<Synonym> headers = {stmtSyn, varSyn};
    std::vector<SynonymValues> values = {stmtValues, varValues};
    return {headers, values};
}

ClauseResult Modifies::allModifiedVariables(PKBFacadeReader& reader) {
    Synonym varSyn = static_cast<Synonym&>(var);

    SynonymValues values{};
    for (Variable currVar : reader.getVariables()) {
        if (!reader.getModifiesStatementsByVariable(currVar).empty()) {
            values.push_back(currVar);
        }
    }

    return {varSyn, values};
}

ClauseResult Modifies::allStmtsThatModifySomeVar(PKBFacadeReader& reader) {
    Synonym stmtSyn = static_cast<Synonym&>(stmt);

    std::unordered_set<Variable> vars = reader.getVariables();
    std::unordered_set<StmtNum> allStmts{};
    for (Variable var : vars) {
        std::unordered_set<StmtNum> stmts = reader.getModifiesStatementsByVariable(var);
        allStmts.insert(stmts.begin(), stmts.end());
    }

    SynonymValues values{};
    for (StmtNum currStmt : filterStatementsByType(reader, stmtSyn.getType(), allStmts)) {
        values.push_back(std::to_string(currStmt));
    }

    return {stmtSyn, values};
}

ClauseResult Modifies::statementsModifyLiteral(PKBFacadeReader& reader) {
    Synonym stmtSyn = static_cast<Synonym&>(stmt);
    Literal varLit = static_cast<Literal&>(var);

    SynonymValues values{};
    std::unordered_set<StmtNum> stmts = reader.getModifiesStatementsByVariable(varLit.getValue());
    for (StmtNum currStmt : filterStatementsByType(reader, stmtSyn.getType(), stmts)) {
        values.push_back(std::to_string(currStmt));
    }

    return {stmtSyn, values};
}

ClauseResult Modifies::variablesModifedByStatement(PKBFacadeReader& reader) {
    Integer stmtInt = static_cast<Integer&>(stmt);
    Synonym varSyn = static_cast<Synonym&>(var);

    SynonymValues values{};
    for (Variable currVar : reader.getModifiesVariablesByStatement(std::stoi(stmtInt.getValue()))) {
        values.push_back(currVar);
    }

    return {varSyn, values};
}

std::unordered_set<StmtNum> Modifies::filterStatementsByType(PKBFacadeReader& reader, DesignEntityType type,
                                                             std::unordered_set<StmtNum> stmts) {
    StatementType mappedStmtType = DESIGN_ENTITY_TYPE_TO_STMT_TYPE_MAP[type];

    if (type == DesignEntityType::STMT) {
        return stmts;
    }

    std::unordered_set<StmtNum> filteredSet{};
    for (StmtNum currStmt : stmts) {
        if (reader.getStatementByStmtNum(currStmt)->type == mappedStmtType) {
            filteredSet.insert(currStmt);
        }
    }
    return filteredSet;
}

bool Modifies::isSimpleResult() const {
    return !stmt.isSynonym() && !var.isSynonym();
}
