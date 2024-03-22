#pragma once

#include "PKB/PKBClient/PKBFacadeReader.h"
#include "Relationship.h"
#include "qps/clauseArguments/ClauseArgument.h"
#include "qps/clauseArguments/Integer.h"
#include "qps/clauses/ClauseResult.h"

class Affects : public Relationship {
private:
    ClauseArgument& affector;
    ClauseArgument& affected;

    /**
     * Check if the result is a simple boolean result.
     * It is a simple boolean result if both arguments are not synonyms.
     * @return true if the result is a simple boolean result, false otherwise.
     */
    bool isSimpleResult() const;

    ClauseResult evaluateSynonymInteger(PKBFacadeReader&);
    ClauseResult evaluateBothIntegers(PKBFacadeReader&);
    ClauseResult evaluateBothWildcards(PKBFacadeReader&);
    ClauseResult evaluateWildcardInteger(PKBFacadeReader&);
    ClauseResult evaluateSynonymWildcard(PKBFacadeReader&);
    ClauseResult evaluateBothSynonyms(PKBFacadeReader&);
protected:
    std::unordered_set<StmtNum> getNextStmtNums(
        const std::unordered_set<std::tuple<Variable, StmtNum>>&, PKBFacadeReader&);
    std::unordered_set<std::tuple<Variable, StmtNum>> getAssignStatements(PKBFacadeReader&);
public:
    Affects(ClauseArgument& affector, ClauseArgument& affected);

    ClauseResult evaluate(PKBFacadeReader&) override;
    bool validateArguments() override;
};
