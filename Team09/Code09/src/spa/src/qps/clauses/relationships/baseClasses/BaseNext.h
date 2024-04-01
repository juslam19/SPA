#pragma once

#include "qps/clauses/relationships/Relationship.h"

using StmtSet = std::unordered_set<StmtNum>;

class BaseNext : public Relationship {
protected:
    std::shared_ptr<ClauseArgument> currentStmt;
    std::shared_ptr<ClauseArgument> nextStmt;

private:
    bool isSimpleResult() const;

    ClauseResult evaluateSynonymInteger(PKBFacadeReader&);
    ClauseResult evaluateSynonymWildcard(PKBFacadeReader&);
    ClauseResult evaluateBothSynonyms(PKBFacadeReader&);

    virtual bool hasNextRelationship(PKBFacadeReader&) = 0;
    virtual StmtSet getNexters(PKBFacadeReader&, const StmtNum&) = 0;
    virtual StmtSet getNextees(PKBFacadeReader&, const StmtNum&) = 0;

public:
    BaseNext(std::shared_ptr<ClauseArgument>, std::shared_ptr<ClauseArgument>);

    ClauseResult evaluate(PKBFacadeReader&) override;
    bool validateArguments() override;
};
