#pragma once

#include <qps/clauseArguments/ClauseArgument.h>

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "PKB/Utils/DataTypes.h"
#include "PKB/Utils/TransitiveClosureUtility.h"

class CallStore {
public:
    void setCallStore(const std::unordered_set<std::pair<Procedure, Procedure>>& callPairs);
    void setCallStmtStore(const std::unordered_set<std::pair<Procedure, StmtNum>>& callStmtPairs);

    std::unordered_set<Procedure> getCaller(Procedure callee);
    std::unordered_set<Procedure> getCallee(Procedure caller);
    std::unordered_set<Procedure> getCallerStar(Procedure callee);
    std::unordered_set<Procedure> getCalleeStar(Procedure caller);
    std::unordered_set<StmtNum> getStmtNumFromCall(Procedure call);
    Procedure getCallFromStmtNum(StmtNum stmtNum);
    bool hasCallRelationship(Procedure caller, Procedure callee);
    bool hasCallRelationship(ClauseArgument& arg1, ClauseArgument& arg2);
    bool hasCallStarRelationship(Procedure caller, Procedure callee);
    bool hasCallStarRelationship(ClauseArgument& arg1, ClauseArgument& arg2);

private:
    std::unordered_map<Procedure, std::unordered_set<Procedure>> callerToCalleeMap;
    std::unordered_map<Procedure, std::unordered_set<Procedure>> calleeToCallerMap;
    std::unordered_map<Procedure, std::unordered_set<Procedure>> callerToCalleeStarMap;
    std::unordered_map<Procedure, std::unordered_set<Procedure>> calleeToCallerStarMap;

    std::unordered_map<Procedure, std::unordered_set<StmtNum>> callToStmtNumberMap;
    std::unordered_map<StmtNum, Procedure> StmtNumberToCallMap;

    void computeTransitiveClosure();
};
