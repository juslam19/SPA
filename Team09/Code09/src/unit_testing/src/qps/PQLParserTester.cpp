#include "PQLParserTester.h"

void PQLParserTester::modifyClauseList(std::vector<std::string>& clauseList) {
    return PQLParser::modifyClauseList(clauseList);
}

std::string PQLParserTester::getQueryClauses(UnparsedQueries unparsedQuery) {
    return PQLParser::getQueryClauses(unparsedQuery);
}

SynonymStore PQLParserTester::parseQueryEntities(std::vector<std::string> unparsedEntities) {
    return PQLParser::parseQueryEntities(unparsedEntities);
}

std::tuple<std::shared_ptr<SelectEntContainer>, std::vector<std::shared_ptr<SuchThatClause>>,
           std::vector<std::shared_ptr<PatternClause>>, std::vector<std::shared_ptr<WithClause>>>
PQLParserTester::parseClauses(const std::vector<std::string>& clauseList) {
    return PQLParser::parseClauses(clauseList);
}

std::shared_ptr<SelectEntContainer> PQLParserTester::parseSelectClause(std::string clauseString) {
    return PQLParser::parseSelectClause(clauseString);
}

std::shared_ptr<SuchThatClause> PQLParserTester::parseSuchThatClauses(std::string clauseString) {
    return PQLParser::parseSuchThatClauses(clauseString);
}

std::shared_ptr<PatternClause> PQLParserTester::parsePatternClauses(std::string clauseString) {
    return PQLParser::parsePatternClauses(clauseString);
}
