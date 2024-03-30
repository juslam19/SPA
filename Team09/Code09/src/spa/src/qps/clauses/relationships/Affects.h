#pragma once

#include "PKB/PKBClient/PKBFacadeReader.h"
#include "Relationship.h"
#include "qps/clauseArguments/ClauseArgument.h"
#include "qps/clauseArguments/Integer.h"
#include "qps/clauses/ClauseResult.h"

// Define AffectsSet
using AffectsSet = std::unordered_set<std::pair<StmtNum, StmtNum>>;

class Affects : public Relationship {
private:
    ClauseArgument& affector;
    ClauseArgument& affected;

    ClauseResult evaluateSynonymInteger(PKBFacadeReader&);
    ClauseResult evaluateBothIntegers(PKBFacadeReader&);
    ClauseResult evaluateBothWildcards(PKBFacadeReader&);
    ClauseResult evaluateWildcardInteger(PKBFacadeReader&);
    ClauseResult evaluateSynonymWildcard(PKBFacadeReader&);
    ClauseResult evaluateBothSynonyms(PKBFacadeReader&);

    AffectsSet generateAffectsRelation(PKBFacadeReader& reader);
    void generateAffectsfromAffector(AffectsSet&, StmtNum&, PKBFacadeReader&);
    void generateAffectsfromAffected(AffectsSet& result, StmtNum& affectedStmtNum, PKBFacadeReader& reader);
    bool hasCommonValue(const std::unordered_set<Variable>&, const std::unordered_set<Variable>&);
    bool checkSynonym(ClauseArgument& clauseArgument);
    bool checkAssign(ClauseArgument& clauseArgument);

public:
    Affects(ClauseArgument& affector, ClauseArgument& affected);

    ClauseResult evaluate(PKBFacadeReader&) override;
    bool validateArguments() override;
};
