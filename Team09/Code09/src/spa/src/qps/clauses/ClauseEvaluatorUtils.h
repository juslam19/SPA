#pragma once
#include <unordered_set>

#include "PKB/PKBClient/PKBFacadeReader.h"
#include "qps/clauseArguments/Synonym.h"
#include "qps/clauses/relationships/Relationship.h"

class ClauseEvaluatorUtils {
public:
    static std::vector<std::string> filterStatementsByType(PKBFacadeReader&, DesignEntityType,
                                                           std::unordered_set<StmtNum>);
    static bool isIdentLiteral(const std::string& str);
    static std::vector<std::vector<std::string>> transpose(std::vector<std::vector<std::string>>);
};
