#include "Query.h"

Query::Query(const std::vector<Synonym>& se, const std::vector<SuchThatClause>& stc,
             const std::vector<PatternClause>& pc)
    : selectEntities(se), suchThatClauses(stc), patternClauses(pc) {}

std::vector<std::string> Query::evaluate(const PKBFacadeReader& pkb) {
    std::vector<Table> clauseTables{};
    for (SuchThatClause stc : suchThatClauses) {
        clauseTables.push_back(stc.evaluate(pkb));
    }

    for (PatternClause pc : patternClauses) {
        clauseTables.push_back(pc.evaluate(pkb));
    }

    if (clauseTables.empty()) {
        return buildSelectTable(pkb).extractResults(selectEntities);
    }

    Table result = clauseTables[0];
    for (size_t i = 1; i < clauseTables.size(); i++) {
        result = result.join(clauseTables[i]);
    }

    return result.extractResults(selectEntities);
}

std::vector<Synonym> Query::getSelectEntities() const {
    return selectEntities;
}

Table Query::buildSelectTable(const PKBFacadeReader& pkb) {
    std::vector<Synonym> header{selectEntities};
    std::vector<ColumnData> columns{};

    for (Synonym entity : selectEntities) {
        ColumnData row{};

        switch (entity.getType()) {
        case DesignEntityType::VARIABLE:
            for (std::string var : pkb.getVariables()) {
                row.push_back(var);
            }
            break;
        case DesignEntityType::CONSTANT:
            for (std::string con : pkb.getConstants()) {
                row.push_back(con);
            }
            break;
        case DesignEntityType::PROCEDURE:
            for (std::string prod : pkb.getProcedures()) {
                row.push_back(prod);
            }
            break;
        }
        columns.push_back(row);
    }

    return Table{header, columns};
}
