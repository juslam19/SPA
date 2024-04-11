#include "Query.h"

#include <qps/clauses/SynonymValuesRetriever.h>
#include <qps/clauses/with/AttributeCollector.h>

Query::Query(const std::vector<Synonym>& se, const std::vector<std::shared_ptr<SuchThatClause>>& stc,
             const std::vector<std::shared_ptr<PatternClause>>& pc, const std::vector<std::shared_ptr<WithClause>>& wc)
    : selectEntities(se), suchThatClauses(stc), patternClauses(pc), withClauses(wc) {}

std::vector<std::string> Query::evaluate(PKBFacadeReader& pkb) const {
    if (hasNoClauses() && isSelectBoolean()) {
        return {QPSConstants::TRUE_STRING};
    }
    const TableManager tableManager{};
    const std::shared_ptr sharedTableManager{std::make_shared<TableManager>(tableManager)};
    const std::shared_ptr sharedPkb{std::make_shared<PKBFacadeReader>(pkb)};
    const std::shared_ptr evalDb{std::make_shared<EvaluationDb>(EvaluationDb{sharedPkb, sharedTableManager})};

    if (!evaluateBooleanClauses(pkb, evalDb)) {
        return getEmptyResult();
    }

    if (!getNonBooleanClauses().empty()) {
        QueryDb queryDb{getNonBooleanClauses()};
        queryDb.loadClausesWithEntities(this->selectEntities);
        if (evaluateAndJoinClauses(tableManager, queryDb, pkb, evalDb); tableManager.isEmpty()) {
            return getEmptyResult();
        }

        while (queryDb.loadNewGroup()) {
            const TableManager nonConnectedTableManager{};
            if (evaluateAndJoinClauses(nonConnectedTableManager, queryDb, pkb, evalDb);
                nonConnectedTableManager.isEmpty()) {
                return getEmptyResult();
            }
        }
    }

    if (buildAndJoinSelectTable(tableManager, pkb); tableManager.isEmpty()) {
        // There are no results to select at all. Return empty result.
        return getEmptyResult();
    }

    if (isSelectBoolean()) {
        return tableManager.isEmpty() ? std::vector{QPSConstants::FALSE_STRING}
                                      : std::vector{QPSConstants::TRUE_STRING};
    }

    projectAttributes(tableManager, pkb);
    return tableManager.extractResults(selectEntities);
}

void Query::projectAttributes(const TableManager& tm, PKBFacadeReader& pkb) const {
    std::vector<Synonym> synonymsWithAttributes{};

    for (Synonym syn : selectEntities) {
        if (syn.getAttr().has_value()) {
            synonymsWithAttributes.push_back(syn);
        }
    }

    tm.projectAttributes(synonymsWithAttributes, headerMatcher, projectSynonymAttributesTransformer(pkb));
}

Synonym Query::headerMatcher(const std::vector<Synonym>& synonyms, Synonym newSynonym) {
    auto it = std::find_if(synonyms.begin(), synonyms.end(), [&newSynonym](const Synonym& existingSynonym) {
        return newSynonym.equalSynonymValue(existingSynonym);
    });

    if (it == synonyms.end()) {
        throw QPSNonSynonymAccessError();
    }
    return *it;
}

ValueTransformer Query::projectSynonymAttributesTransformer(PKBFacadeReader& pkb) {
    return [&pkb](const Synonym& synonym, const SynonymValue& value) -> SynonymValue {
        return AttributeCollector::collect(pkb, synonym, value);
    };
}

bool Query::hasNoClauses() const {
    return suchThatClauses.empty() && patternClauses.empty() && withClauses.empty();
}

bool Query::isSelectBoolean() const {
    return selectEntities.empty();
}

std::vector<std::string> Query::getEmptyResult() const {
    return this->isSelectBoolean() ? std::vector{QPSConstants::FALSE_STRING} : std::vector<std::string>{};
}

void Query::evaluateAndJoinClauses(const TableManager& tm, QueryDb& db, PKBFacadeReader& pkb,
                                   const std::shared_ptr<EvaluationDb>& evalDb) {
    OptionalQueryClause next = db.next();
    while (next.has_value() && !tm.isEmpty()) {
        auto clause = next->get();
        ClauseResult res = clause->runEvaluation(pkb, evalDb);
        tm.join(res);
        next = db.next();
    }
}

bool Query::evaluateBooleanClauses(PKBFacadeReader& pkb, const std::shared_ptr<EvaluationDb>& evalDb) const {
    for (auto stc : suchThatClauses) {
        if (stc->isBooleanResult() && !stc->runEvaluation(pkb, evalDb).getBoolean()) {
            return false;
        }
    }
    for (auto wc : withClauses) {
        if (wc->isBooleanResult() && !wc->runEvaluation(pkb, evalDb).getBoolean()) {
            return false;
        }
    }
    return true;
}

std::vector<QueryClausePtr> Query::getNonBooleanClauses() const {
    std::vector<QueryClausePtr> nonBooleanClauses{};

    // Convert patternClauses to shared_ptr
    for (auto pc : patternClauses) {
        nonBooleanClauses.push_back(pc);
    }

    // Convert suchThatClauses to shared_ptr if they are not boolean result
    for (auto stc : suchThatClauses) {
        if (!stc->isBooleanResult()) {
            nonBooleanClauses.push_back(stc);
        }
    }

    for (auto wc : withClauses) {
        if (!wc->isBooleanResult()) {
            nonBooleanClauses.push_back(wc);
        }
    }
    return nonBooleanClauses;
}

void Query::buildAndJoinSelectTable(const TableManager& tm, PKBFacadeReader& pkb) const {
    if (isSelectBoolean())
        return;

    std::vector<Synonym> selectEntitiesWithoutAttributes{};

    for (const Synonym& syn : selectEntities) {
        const Synonym synWithoutAttribute = syn.getAttr().has_value() ? syn.getWithoutAttribute() : syn;

        // We only 'top-up' synonyms whose values that are not currently in the table.
        if (!tm.containsHeader(synWithoutAttribute)) {
            selectEntitiesWithoutAttributes.push_back(synWithoutAttribute);
        }
    }

    if (!selectEntitiesWithoutAttributes.empty()) {
        const ClauseResult cr{SynonymValuesRetriever::retrieveAsClauseResult(pkb, selectEntitiesWithoutAttributes)};
        tm.join(cr);
    }
}
