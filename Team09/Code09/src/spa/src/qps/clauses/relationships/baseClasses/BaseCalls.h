#pragma once

#include "qps/clauses/relationships/Relationship.h"

using ProcedureSet = std::unordered_set<Procedure>;

class BaseCalls : public Relationship {
protected:
    std::shared_ptr<ClauseArgument> caller;
    std::shared_ptr<ClauseArgument> callee;

private:
    bool isSimpleResult() const;

    ClauseResult evaluateCalleeSynonym(PKBFacadeReader&);
    ClauseResult evaluateCallerSynonym(PKBFacadeReader&);
    ClauseResult evaluateBothSynonyms(PKBFacadeReader&);

    virtual bool hasCallRelationship(PKBFacadeReader&) = 0;
    virtual ProcedureSet getCallee(PKBFacadeReader&, const Procedure&) = 0;
    virtual ProcedureSet getCaller(PKBFacadeReader&, const Procedure&) = 0;

public:
    BaseCalls(std::shared_ptr<ClauseArgument>, std::shared_ptr<ClauseArgument>);

    ClauseResult evaluate(PKBFacadeReader&) override;
    ClauseResult evaluate(PKBFacadeReader&, const std::shared_ptr<EvaluationDb>&) override;
    bool validateArguments() override;
};
