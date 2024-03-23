#include "Affects.h"

#include "../ClauseEvaluatorUtils.h"

Affects::Affects(ClauseArgument& affector, ClauseArgument& affected)
    : affector(affector), affected(affected) {}

bool Affects::validateArguments() {
    if (affector.isSynonym()) {
        Synonym first = dynamic_cast<Synonym&>(affector);
        if (first.getType() != DesignEntityType::ASSIGN) {
            return false;
        }
    }
    if (affected.isSynonym()) {
        Synonym second = dynamic_cast<Synonym&>(affected);
        if (second.getType() != DesignEntityType::ASSIGN) {
            return false;
        }
    }
    return true;
}

/**
 * Similar logic:
 * Bothsynonyms/BothWildcards
 * SynonymInteger/WildcardInteger
 * BothIntegers
 * SynonymWildcard
*/
ClauseResult Affects::evaluate(PKBFacadeReader& reader) {
    /**
     * Returns TRUE/FALSE
    */
    if (affector.isInteger() && affected.isInteger()) {
        return evaluateBothIntegers(reader);
    }
    if (affector.isWildcard() && affected.isWildcard()) {
        return evaluateBothWildcards(reader);
    }
    if ((affector.isWildcard() && affected.isInteger()) || (affector.isInteger() && affected.isWildcard())) {
        return evaluateWildcardInteger(reader);
    }

    /**
     * Returns either s1, s2, or both
    */
    if ((affector.isInteger() && affected.isSynonym()) || (affector.isSynonym() && affected.isInteger())) {
        return evaluateSynonymInteger(reader);
    }
    if ((affector.isWildcard() && affected.isSynonym()) || (affector.isSynonym() && affected.isWildcard())) {
        return evaluateSynonymWildcard(reader);
    }
    return evaluateBothSynonyms(reader);
}

/**
 * Helper function
*/
std::unordered_set<std::pair<Variable, StmtNum>> Affects::getAssignStatements(PKBFacadeReader& reader) {
    std::unordered_set<Variable> allVar = reader.getVariables();
    std::unordered_set<std::pair<Variable, StmtNum>> varAndAffectorStmtList{};
    for (Variable var : allVar) {
        std::unordered_set<StmtNum> allStmts = reader.getModifiesStatementsByVariable(var);
        std::unordered_set<StmtNum> assignStmts = ClauseEvaluatorUtils::filterStatementsByType(
                                                    reader, DesignEntityType::ASSIGN, allStmts);
        for (StmtNum assignStmt : assignStmts) {
            varAndAffectorStmtList.insert(std::make_pair(var, assignStmt));
        }
    }
    return varAndAffectorStmtList;
}

/**
 * Helper function
*/
std::unordered_set<StmtNum> Affects::getNextStmtNums(
        const std::pair<Variable, StmtNum>& varAndAffectorStmt, PKBFacadeReader& reader) {
    std::unordered_set<StmtNum> nextStmtNums{};
    // Get NextStar Control Flow - all StmtNums that follow that assign statement
    StmtNum affectorStmtNum = std::get<StmtNum>(varAndAffectorStmt);
    std::unordered_set<StmtNum> CFAffector = reader.getNexterStar(affectorStmtNum);
    nextStmtNums.insert(CFAffector.begin(), CFAffector.end());
    return nextStmtNums;
}

ClauseResult Affects::evaluateWildcardInteger(PKBFacadeReader& reader) {
    bool affectorIsInteger = affector.isInteger();
    Integer integer = affectorIsInteger ? dynamic_cast<Integer&>(affector) : dynamic_cast<Integer&>(affected);
    StmtNum stmtNum = std::stoi(integer.getValue());

    /**
     * Get control flow from established statement number
    */
    std::unordered_set<StmtNum> nextStmtNums =
        affectorIsInteger ? reader.getNexterStar(stmtNum) : reader.getNexteeStar(stmtNum);

    /**
     * If affectorIsInteger: start from affector statement and work downwards
     * Else: start from affected statement and work upwards
    */
    std::optional<Stmt> stmt = reader.getStatementByStmtNum(stmtNum);
    if (!stmt.has_value() || stmt.value().type != StatementType::ASSIGN) {
        return false;
    }

    if (affectorIsInteger) {
        std::unordered_set<Variable> modifiedVariables = reader.getModifiesVariablesByStatement(stmtNum);
        for (Variable modifiedVariable : modifiedVariables) {
            for (StmtNum nextStmtNum : nextStmtNums) {
                std::optional<Stmt> nextStmt = reader.getStatementByStmtNum(nextStmtNum);
                // Checking if uses the affector variable
                for (Variable currVar : reader.getUsesVariablesByStatement(nextStmtNum)) {
                    if (!nextStmt.has_value()) {
                        return false;
                    } else {
                        if (nextStmt.value().type == StatementType::ASSIGN && currVar == modifiedVariable) {
                            return true;
                        }
                    }
                }
                // Checking if modifies the affector variable AND not nextStmtNum
                for (Variable currVar : reader.getModifiesVariablesByStatement(nextStmtNum)) {
                    if (currVar == modifiedVariable) {
                        return false;
                    }
                }
            }
        }
        return false;
    } else {
        std::unordered_set<Variable> usesVariables = reader.getUsesVariablesByStatement(stmtNum);
        for (Variable usesVariable : usesVariables) {
            for (StmtNum nextStmtNum : nextStmtNums) {
                std::optional<Stmt> nextStmt = reader.getStatementByStmtNum(nextStmtNum);
                // Checking if modified affected variable
                if (!nextStmt.has_value()) {
                    return false;
                } else {
                    for (Variable modifiedVar : reader.getModifiesVariablesByStatement(nextStmtNum)) {
                        if (modifiedVar == usesVariable) {
                            // If statement assign then mark as value element
                            if (nextStmt.value().type == StatementType::ASSIGN) {
                                return true;
                            }
                            return false;
                        }
                    }
                }
            }
        }
        return false;
    }
}

ClauseResult Affects::evaluateBothIntegers(PKBFacadeReader& reader) {
    Integer affectorInt = dynamic_cast<Integer&>(affector);
    Integer affectedInt = dynamic_cast<Integer&>(affected);

    if (affectorInt == affectedInt) {
        return false;
    }

    StmtNum affectorStmtNum = std::stoi(affectorInt.getValue());
    StmtNum affectedStmtNum = std::stoi(affectedInt.getValue());

    std::optional<Stmt> affectorStmt = reader.getStatementByStmtNum(affectorStmtNum);
    std::optional<Stmt> affectedStmt = reader.getStatementByStmtNum(affectedStmtNum);
    if (!affectorStmt.has_value() || !affectedStmt.has_value() ||
            (affectorStmt.value().type != StatementType::ASSIGN) ||
            (affectedStmt.value().type != StatementType::ASSIGN)) {
        return false;
    }

    bool usesModifiedVariable = false;

    /**
     * Check that affectedStatement Uses Modified variable
    */
    std::unordered_set<Variable> affectorVariables = reader.getModifiesVariablesByStatement(affectorStmtNum);
    std::unordered_set<Variable> affectedVariables = reader.getUsesVariablesByStatement(affectedStmtNum);
    for (Variable affectorVariable: affectorVariables) {
        for (Variable affectedVariable: affectedVariables) {
            if (affectorVariable == affectedVariable) {
                usesModifiedVariable = true;
            }
        }
    }

    /**
     * Check that Uses Modified variable is not Modified inbetween
    */
    std::unordered_set<StmtNum> nextStmtNums = reader.getNexterStar(affectorStmtNum);

    /**
     * For each Nextee:
     * - for all statements, CHECK that does not modify the variable
     * -- if modified, mark notModifiedBetween as false, and exit loop
     * -- else if goal statement, exit loop
    */
    bool modifiedBetween = false;
    for (StmtNum nextStmtNum : nextStmtNums) {
        if ((nextStmtNum == affectedStmtNum) || modifiedBetween) {
            break;
        } else {
            std::unordered_set<Variable> nextStmtVariables = reader.getModifiesVariablesByStatement(nextStmtNum);
            for (Variable nextStmtVariable: nextStmtVariables) {
                for (Variable affectorVariable: affectorVariables) {
                    if (nextStmtVariable == affectorVariable) {
                        modifiedBetween = true;
                        break;
                    }
                }
                if (modifiedBetween) {
                    break;
                }
            }
        }
    }
    return usesModifiedVariable && !(modifiedBetween);
}

ClauseResult Affects::evaluateSynonymWildcard(PKBFacadeReader& reader) {
    bool affectorIsSynonym = affector.isSynonym();
    Synonym syn = affectorIsSynonym ? dynamic_cast<Synonym&>(affector) : dynamic_cast<Synonym&>(affected);

    SynonymValues values{};

    std::unordered_set<std::pair<Variable, StmtNum>> varAndAffectorStmtList = getAssignStatements(reader);
    

    /**
     * For each pair of modified variable and assign statement:
     * For each CF next assign statement:
     * - iterate through variables used by the CF next assign statement,
     * 
     * - for all statements, CHECK that does not modify the variable
     * -- if modified, exit loop for that affecter statement
     * - else if ASSIGN statement, CHECK that currVariable equals modified variable
     * -- if yes, push back values
    */
    for (const std::pair<Variable, StmtNum>& varAndAffectorStmt : varAndAffectorStmtList) {

        std::unordered_set<StmtNum> nextStmtNums = getNextStmtNums(varAndAffectorStmt, reader);
        
        StmtNum stmtNum = std::get<StmtNum>(varAndAffectorStmt);
        Variable variable = std::get<Variable>(varAndAffectorStmt);
        bool modified = false;
        for (StmtNum nextStmtNum : nextStmtNums) {
            std::optional<Stmt> nextStmt = reader.getStatementByStmtNum(nextStmtNum);
            if (modified) {
                break;
            }
            // Checking if uses the affector variable
            std::unordered_set<Variable> currVarList = reader.getUsesVariablesByStatement(nextStmtNum);
            for (Variable currVar : currVarList) {
                if (nextStmt.has_value() &&
                        nextStmt.value().type == StatementType::ASSIGN &&
                        currVar == variable) {
                    if (affectorIsSynonym) {
                        values.push_back(std::to_string(stmtNum));
                    } else {
                        values.push_back(std::to_string(nextStmtNum));
                    }
                }
            }
            // Checking if modifies the affector variable AND not nextStmtNum
            for (Variable currVar : reader.getModifiesVariablesByStatement(nextStmtNum)) {
                if (currVar == variable) {
                    modified = true;
                    break;  // Break out of inner loop for current affector statement
                }
            }
        }
    }
    return {syn, values};
}

ClauseResult Affects::evaluateSynonymInteger(PKBFacadeReader& reader) {
    bool affectorIsInteger = affector.isInteger();
    Synonym syn = affectorIsInteger ? dynamic_cast<Synonym&>(affected) : dynamic_cast<Synonym&>(affector);
    Integer integer = affectorIsInteger ? dynamic_cast<Integer&>(affector) : dynamic_cast<Integer&>(affected);
    StmtNum stmtNum = std::stoi(integer.getValue());

    std::optional<Stmt> stmt = reader.getStatementByStmtNum(stmtNum);
    if (!stmt.has_value() || stmt.value().type != StatementType::ASSIGN) {
        return {syn, {}};
    }

    SynonymValues values{};

    /**
     * Get control flow from established statement number
    */
    std::unordered_set<StmtNum> nextStmtNums =
        affectorIsInteger ? reader.getNexterStar(stmtNum) : reader.getNexteeStar(stmtNum);

    /**
     * If !affectorIsSynonym:
     * - start from affector statement and work downwards
     * Else:
     * - start from affected statement and work upwards
    */
    if (affectorIsInteger) {
        std::unordered_set<Variable> modifiedVariables = reader.getModifiesVariablesByStatement(stmtNum);
        for (Variable modifiedVariable : modifiedVariables) {
            bool modified = false;
            for (StmtNum nextStmtNum : nextStmtNums) {
                if (modified) {
                    break;
                }
                std::optional<Stmt> nextStmt = reader.getStatementByStmtNum(nextStmtNum);
                // Checking if uses the affector variable
                for (Variable currVar : reader.getUsesVariablesByStatement(nextStmtNum)) {
                    if (nextStmt.has_value() && nextStmt.value().type == StatementType::ASSIGN && currVar == modifiedVariable) {
                        values.push_back(std::to_string(nextStmtNum));
                    }
                }
                // Checking if modifies the affector variable AND not nextStmtNum
                for (Variable currVar : reader.getModifiesVariablesByStatement(nextStmtNum)) {
                    if (currVar == modifiedVariable) {
                        modified = true;
                        break;  // Break out of inner loop for current affector statement
                    }
                }
            }
        }
    } else {
        std::unordered_set<Variable> usesVariables = reader.getUsesVariablesByStatement(stmtNum);
        for (Variable usesVariable : usesVariables) {
            bool modified = false;
            for (StmtNum nextStmtNum : nextStmtNums) {
                if (modified) {
                    break;
                }
                std::optional<Stmt> nextStmt = reader.getStatementByStmtNum(nextStmtNum);
                // Checking if modified affected variable
                for (Variable modifiedVar : reader.getModifiesVariablesByStatement(nextStmtNum)) {
                    if (modifiedVar == usesVariable) {
                        // If statement assign then mark as value element
                        if (nextStmt.has_value() && nextStmt.value().type == StatementType::ASSIGN) {
                            values.push_back(std::to_string(nextStmtNum));
                        }
                        modified = true;
                        break;
                    }
                }
            }
        }
    }
    return {syn, values};
}

ClauseResult Affects::evaluateBothSynonyms(PKBFacadeReader& reader) {
    Synonym affectorSyn = dynamic_cast<Synonym&>(affector);
    Synonym affectedSyn = dynamic_cast<Synonym&>(affected);
    std::vector<Synonym> synonyms{affectorSyn, affectedSyn};

    if (affectorSyn == affectedSyn) {
        return {synonyms, {}};
    }

    SynonymValues affectorValues{};
    SynonymValues affectedValues{};

    /**
     * 1. Get assign statements that modify their variables
     * Get statements that modify variables (LHS)
     * Then filter down to assign statements
     * Make pair of modified variable and assign statement
    */
    std::unordered_set<std::pair<Variable, StmtNum>> varAndAffectorStmtList = getAssignStatements(reader);
    /**
     * 2. For each pair of modified variable and assign statement
     * - Get the control flow statements that follow it (like NextStar)
     * - Get the CF with all the statements
     * - Get separate list with assign statements
    */
    

    /**
     * For each pair of modified variable and assign statement:
     * For each CF next assign statement:
     * - iterate through variables used by the CF next assign statement,
     * 
     * - if not ASSIGN statement, CHECK that does not modify the variable
     * -- if modified, exit loop for that affecter statement
     * - else if ASSIGN statement, CHECK that currVariable equals modified variable
     * -- if yes, push back values
    */
    for (const std::pair<Variable, StmtNum>& varAndAffectorStmt : varAndAffectorStmtList) {
        std::unordered_set<StmtNum> nextStmtNums = getNextStmtNums(varAndAffectorStmt, reader);
        StmtNum stmtNum = std::get<StmtNum>(varAndAffectorStmt);
        Variable variable = std::get<Variable>(varAndAffectorStmt);
        bool modified = false;
        for (StmtNum nextStmtNum : nextStmtNums) {
            std::optional<Stmt> nextStmt = reader.getStatementByStmtNum(nextStmtNum);
            if (modified) {
                break;
            }
            // Checking if uses the affector variable
            for (Variable currVar : reader.getUsesVariablesByStatement(nextStmtNum)) {
                if (nextStmt.has_value() && nextStmt.value().type == StatementType::ASSIGN && currVar == variable) {
                    affectorValues.push_back(std::to_string(stmtNum));
                    affectedValues.push_back(std::to_string(nextStmtNum));
                }
            }
            // Checking if modifies the affector variable AND not nextStmtNum
            for (Variable currVar : reader.getModifiesVariablesByStatement(nextStmtNum)) {
                if (currVar == variable) {
                    modified = true;
                    break;  // Break out of inner loop for current affector statement
                }
            }
        }
    }
    std::vector<Synonym> headers = {affectorSyn, affectedSyn};
    std::vector<SynonymValues> values = {affectorValues, affectedValues};
    return {headers, values};
}

ClauseResult Affects::evaluateBothWildcards(PKBFacadeReader& reader) {
    std::unordered_set<std::pair<Variable, StmtNum>> varAndAffectorStmtList = getAssignStatements(reader);
    

    for (const std::pair<Variable, StmtNum>& varAndAffectorStmt : varAndAffectorStmtList) {
        std::unordered_set<StmtNum> nextStmtNums = getNextStmtNums(varAndAffectorStmt, reader);
        StmtNum stmtNum = std::get<StmtNum>(varAndAffectorStmt);
        Variable variable = std::get<Variable>(varAndAffectorStmt);
        bool modified = false;
        for (StmtNum nextStmtNum : nextStmtNums) {
            std::optional<Stmt> stmt = reader.getStatementByStmtNum(stmtNum);
            if (modified) {
                break;
            }
            // Checking if uses the affector variable
            for (Variable currVar : reader.getUsesVariablesByStatement(nextStmtNum)) {
                if (!stmt.has_value()) {
                    return false;
                } else { 
                    if (stmt.value().type == StatementType::ASSIGN && currVar == variable) {
                        return true;
                    }
                }
            }
            // Checking if modifies the affector variable AND not nextStmtNum
            for (Variable currVar : reader.getModifiesVariablesByStatement(nextStmtNum)) {
                if (currVar == variable) {
                    modified = true;
                    break;  // Break out of inner loop for current affector statement
                }
            }
        }
    }
    return false;
}
