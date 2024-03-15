#pragma once

#include <unordered_set>

#include "PKB/PKBClient/PKBFacadeReader.h"
#include "Table.h"
#include "TableManager.h"
#include "qps/clauseArguments/Synonym.h"
#include "qps/clauses/PatternClause.h"
#include "qps/clauses/SuchThatClause.h"

using QueryClausePtr = std::shared_ptr<QueryClause>;

struct ArrangedClauses {
    std::vector<std::vector<QueryClausePtr>> selectConnectedClauses;
    std::vector<std::vector<QueryClausePtr>> nonSelectConnectedClauses;
};

namespace {
const std::string& TRUE_STRING = "TRUE";
const std::string& FALSE_STRING = "FALSE";
}  // namespace

class Query {
public:
    Query(const std::vector<Synonym>&, const std::vector<SuchThatClause>&, const std::vector<PatternClause>&);
    std::vector<std::string> evaluate(PKBFacadeReader&) const;

private:
    std::vector<Synonym> selectEntities;
    std::vector<SuchThatClause> suchThatClauses;
    std::vector<PatternClause> patternClauses;

    static bool evaluateAndJoinClauses(const TableManager& tm,
                                       const std::vector<std::vector<QueryClausePtr>>& connectedClausesList,
                                       PKBFacadeReader& pkb);

    Table buildSelectTable(const PKBFacadeReader&) const;
    std::vector<QueryClausePtr> getNonBooleanClauses() const;
    bool evaluateBooleanClauses(PKBFacadeReader&) const;
    bool containsSelectSynonyms(QueryClausePtr) const;
    std::vector<std::vector<QueryClausePtr>> splitIntoConnectedSynonyms() const;
    ArrangedClauses arrangeClauses() const;
    bool hasNoClauses() const;
    bool isSelectBoolean() const;
};
