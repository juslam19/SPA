#pragma once

#include "Relationship.h"
#include "pkb/PKBClient/PKBFacadeReader.h"
#include "qps/clauseArguments/ClauseArgument.h"
#include "qps/clauseArguments/Integer.h"
#include "qps/clauses/ClauseResult.h"

class Modifies : public Relationship {
private:
    std::shared_ptr<ClauseArgument> modifier;
    std::shared_ptr<ClauseArgument> var;

    /**
     * Check if the result is a simple boolean result.
     * It is a simple boolean result if both arguments are not synonyms.
     * @return true if the result is a simple boolean result, false otherwise.
     */
    bool isSimpleResult() const;

    ClauseResult evaluateModifierSynonym(PKBFacadeReader&);
    ClauseResult variablesModifedByStatement(PKBFacadeReader&);
    ClauseResult variablesModifiedByProcedure(PKBFacadeReader& reader);
    ClauseResult evaluateBothSynonyms(PKBFacadeReader&, EvaluationDb& evalDb);

public:
    Modifies(std::shared_ptr<ClauseArgument>, std::shared_ptr<ClauseArgument>);

    ClauseResult evaluate(PKBFacadeReader&, EvaluationDb&) override;
    bool validateArguments() override;
};
