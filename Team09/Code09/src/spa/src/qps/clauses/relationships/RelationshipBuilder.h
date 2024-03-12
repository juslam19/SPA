#pragma once

#include <unordered_map>

#include "Follows.h"
#include "FollowsStar.h"
#include "Modifies.h"
#include "Next.h"
#include "NextStar.h"
#include "PKB/Utils/DataTypes.h"
#include "Parent.h"
#include "ParentStar.h"
#include "Relationship.h"
#include "Uses.h"
#include "qps/QPSConstants.h"
#include "qps/clauseArguments/Synonym.h"
#include "qps/clauses/ClauseResult.h"
#include "qps/exceptions/Exception.h"

static const std::unordered_map<std::string, RelationshipType> RELATIONSHIP_TYPE_MAP = {
    {QPSConstants::FOLLOWS, RelationshipType::FOLLOWS},
    {QPSConstants::FOLLOWS_STAR, RelationshipType::FOLLOWS_STAR},
    {QPSConstants::PARENT, RelationshipType::PARENT},
    {QPSConstants::PARENT_STAR, RelationshipType::PARENT_STAR},
    {QPSConstants::USES, RelationshipType::USES},
    {QPSConstants::MODIFIES, RelationshipType::MODIFIES},
    {QPSConstants::NEXT, RelationshipType::NEXT},
    {QPSConstants::NEXT_STAR, RelationshipType::NEXT_STAR}};  // update here

class RelationshipBuilder {
public:
    static RelationshipType determineRelationshipType(const std::string& type);

    static std::unique_ptr<Relationship> createRelationship(RelationshipType type, ClauseArgument& firstArg,
                                                            ClauseArgument& secondArg);
};
