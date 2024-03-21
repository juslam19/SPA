#include "WhilePatternStore.h"

void WhilePatternStore::initialiseStore(std::function<bool(std::string, std::string)> exactMatchFP,
                                        std::function<bool(std::string, std::string)> partialMatchFP,
                                        std::unordered_set<std::pair<StmtNum, std::string>> whilePatternPairs) {
    this->exactMatchFP = exactMatchFP;
    this->partialMatchFP = partialMatchFP;
    setWhilePatterns(whilePatternPairs);
}

void WhilePatternStore::setWhilePatterns(const std::unordered_set<std::pair<StmtNum, std::string>>& whilePatternPairs) {
    for (const auto& pattern : whilePatternPairs) {
        const auto& variable = pattern.second;
        whilePatternsMap[pattern.first].insert(variable);
    }
}

bool WhilePatternStore::hasExactPattern(StmtNum stmtNum, std::string arg) {
    return applyWhilePatternFunction(exactMatchFP, stmtNum, arg);
}

bool WhilePatternStore::hasPartialPattern(StmtNum stmtNum, std::string arg) {
    return applyWhilePatternFunction(partialMatchFP, stmtNum, arg);
}

bool WhilePatternStore::applyWhilePatternFunction(std::function<bool(std::string, std::string)> function,
                                                  StmtNum stmtNum, std::string arg) {
    auto it = whilePatternsMap.find(stmtNum);

    // Check if stmtNum is found
    if (it != whilePatternsMap.end()) {
        // Retrieve all the control variables with stmtNum
        const auto& vars = it->second;
        for (const auto& var : vars) {
            return function(var, arg);
        }
    }
    return false;
}
