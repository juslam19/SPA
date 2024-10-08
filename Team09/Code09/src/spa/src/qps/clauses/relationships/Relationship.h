#pragma once

#include <unordered_map>

#include "pkb/PKBClient/PKBFacadeReader.h"
#include "pkb/Utils/DataTypes.h"
#include "qps/clauseArguments/Synonym.h"
#include "qps/clauses/ClauseResult.h"
#include "qps/evaluator/EvaluationDb.h"

enum class RelationshipType {
    FOLLOWS,
    FOLLOWS_STAR,
    PARENT,
    PARENT_STAR,
    MODIFIES,
    USES,
    NEXT,
    NEXT_STAR,
    CALLS,
    CALLS_STAR,
    AFFECTS
};

static std::unordered_map<DesignEntityType, StatementType> DESIGN_ENTITY_TYPE_TO_STMT_TYPE_MAP = {
    {DesignEntityType::ASSIGN, StatementType::ASSIGN}, {DesignEntityType::WHILE, StatementType::WHILE},
    {DesignEntityType::IF, StatementType::IF},         {DesignEntityType::CALL, StatementType::CALL},
    {DesignEntityType::READ, StatementType::READ},     {DesignEntityType::PRINT, StatementType::PRINT},
};

class Relationship {
public:
    virtual ~Relationship(){};
    virtual ClauseResult evaluate(PKBFacadeReader& reader, EvaluationDb& evalDb) = 0;
    virtual bool validateArguments() = 0;
};
