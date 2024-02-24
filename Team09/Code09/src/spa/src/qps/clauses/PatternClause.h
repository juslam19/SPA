#pragma once

#include "QueryClause.h"
#include "qps/clauseArguments/ExpressionSpec.h"
#include "qps/clauseArguments/Synonym.h"

class PatternClause : public QueryClause {
private:
    const ClauseArgument* assignSynonym;
    const ClauseArgument* firstArg;
    const ClauseArgument* secondArg;

public:
    PatternClause(const ClauseArgument*, const ClauseArgument*, const ClauseArgument*);
    ClauseType getType();
    bool equals(const QueryClause&);
    void print();
    // ClauseResult evaluate(const PKBFacadeReader&) override;
};
