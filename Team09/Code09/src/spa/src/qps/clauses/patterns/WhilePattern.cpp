#include "WhilePattern.h"

WhilePattern::WhilePattern(ClauseArgument* whileSyn, std::vector<ClauseArgument*> args)
    : whileSyn(*whileSyn), arguments(args) {}

ClauseResult WhilePattern::evaluate(PKBFacadeReader& reader) {
    if (arguments[0]->isSynonym()) {
        return evaluateFirstArgSyn(reader);
    } else if (arguments[0]->isLiteral()) {
        return evaluateFirstArgLiteral(reader);
    } else {
        return evaluateFirstArgWildcard(reader);
    }
}

bool WhilePattern::validateArguments() {
    if (arguments.size() != 2) {
        return false;
    }
    if (!arguments[1]->isWildcard()) {
        return false;
    }
    return true;
}

ClauseResult WhilePattern::evaluateFirstArgSyn(PKBFacadeReader& reader) {
    Synonym wSyn = dynamic_cast<Synonym&>(whileSyn);
    Synonym fSyn = dynamic_cast<Synonym&>(*arguments[0]);

    std::unordered_set<Stmt> whileStmts = reader.getStatementsByType(StatementType::WHILE);
    std::unordered_set<Variable> allVars = reader.getVariables();

    std::vector<std::string> stmtNumbers = {};
    std::vector<std::string> synValues = {};

    for (Stmt stmt : whileStmts) {
        for (Variable var : allVars) {
            if (reader.hasWhilePattern(stmt.stmtNum, var)) {
                // keep track of syn and stmt
                stmtNumbers.push_back(std::to_string(stmt.stmtNum));
                synValues.push_back(var);
            }
        }
    }

    std::vector<Synonym> returnSyn = {wSyn, fSyn};
    std::vector<SynonymValues> returnSynValues = {stmtNumbers, synValues};
    return {returnSyn, returnSynValues};
}

ClauseResult WhilePattern::evaluateFirstArgLiteral(PKBFacadeReader& reader) {
    Synonym wSyn = dynamic_cast<Synonym&>(whileSyn);

    std::unordered_set<Stmt> whileStmts = reader.getStatementsByType(StatementType::WHILE);

    std::vector<std::string> stmtNumbers = {};

    for (Stmt stmt : whileStmts) {
        if (reader.hasWhilePattern(stmt.stmtNum, arguments[0]->getValue())) {
            stmtNumbers.push_back(std::to_string(stmt.stmtNum));
        }
    }

    return {wSyn, stmtNumbers};
}

ClauseResult WhilePattern::evaluateFirstArgWildcard(PKBFacadeReader& reader) {
    Synonym wSyn = dynamic_cast<Synonym&>(whileSyn);

    std::unordered_set<Stmt> whileStmts = reader.getStatementsByType(StatementType::WHILE);
    std::unordered_set<Variable> allVars = reader.getVariables();

    std::vector<std::string> stmtNumbers = {};

    for (Stmt stmt : whileStmts) {
        for (Variable var : allVars) {
            if (reader.hasWhilePattern(stmt.stmtNum, var)) {
                stmtNumbers.push_back(std::to_string(stmt.stmtNum));
            }
        }
    }

    return {wSyn, stmtNumbers};
}
