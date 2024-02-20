#pragma once

#include <string>
#include <vector>

#include "../exceptions/SyntaxError.h"
#include "BasicToken.h"
#include "LexicalAnalyzer.h"
#include "Token.h"

class SpTokenizer {
private:
    LexicalAnalyzer lexicalAnalyzer;
    SyntaxValidator syntaxValidator;
    int stmtNum = 0;
    const int NO_STMT_NUM = -1;
    int notStmt = 0;

public:
    /**
     * @brief Takes in the source code as a vector of strings and turns into a
     * vector of Tokens.
     */
    std::vector<Token> tokenize(std::vector<std::string>);

    /**
     * @brief Assign specific token types and statement numbers to each token.
     * Procedure definitions, else keywords on a line and close brackets are
     * assigned statement number -1.
     */
    std::vector<Token> assignTokens(std::vector<BasicToken*> input);
    Token assignKeyword(BasicToken* bt);
    Token assignSymbol(BasicToken* bt);
};
